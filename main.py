from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.security import HTTPAuthorizationCredentials
from auth import create_token, get_current_user
from models import Message
import json

app = FastAPI()

class ConnectionManager:
    def __init__(self):
        self.active_connections: list[WebSocket] = []
        self.topics = {"topic1": [], "topic2": []}

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)

    def disconnect(self, websocket: WebSocket):
        self.active_connections.remove(websocket)
        for topic in self.topics.values():
            if websocket in topic:
                topic.remove(websocket)

    async def send_personal_message(self, message: str, websocket: WebSocket):
        await websocket.send_text(message)

    async def broadcast(self, message: str, topic: str):
        for connection in self.topics[topic]:
            await connection.send_text(message)

    def subscribe(self, websocket: WebSocket, topic: str):
        if websocket not in self.topics[topic]:
            self.topics[topic].append(websocket)

manager = ConnectionManager()

@app.post("/token")
async def login():
    # Simula una autenticación de usuario y genera un token JWT
    token = create_token({"username": "user1"})
    return {"access_token": token}

@app.websocket("/ws/{token}")
async def websocket_endpoint(websocket: WebSocket, token: str):
    user = get_current_user(credentials=HTTPAuthorizationCredentials(scheme="Bearer", credentials=token))
    await manager.connect(websocket)
    try:
        while True:
            data = await websocket.receive_text()
            message = json.loads(data)
            msg_obj = Message(**message)
            manager.subscribe(websocket, msg_obj.topic)
            await manager.broadcast(f"{user['username']}: {msg_obj.content}", msg_obj.topic)
    except WebSocketDisconnect:
        manager.disconnect(websocket)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
