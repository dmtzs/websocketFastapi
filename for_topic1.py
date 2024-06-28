import asyncio
import websockets
import json
import httpx


async def authenticate_and_connect():
    token = await get_token()
    websocket_url = f"ws://localhost:8000/ws/{token}"
    async with websockets.connect(websocket_url) as websocket:
        await subscribe_and_listen_topic1(websocket)

async def get_token():
    async with httpx.AsyncClient() as client:
        response = await client.post("http://localhost:8000/token")
        response.raise_for_status()
        data = response.json()
        return data['access_token']

async def subscribe_and_listen_topic1(websocket):
    # Suscribirse a topic1
    await websocket.send(json.dumps({"topic": "topic1", "content": "Subscribing to topic1"}))
    try:
        while True:
            message = await websocket.recv()
            print(f"Received message on topic1: {message}")
    except websockets.ConnectionClosed:
        print("Connection closed")

if __name__ == "__main__":
    asyncio.run(authenticate_and_connect())
