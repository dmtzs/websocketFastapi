import asyncio
import websockets
import json

async def authenticate_and_connect():
    token = await get_token()
    websocket_url = f"ws://localhost:8000/ws/{token}"
    async with websockets.connect(websocket_url) as websocket:
        await subscribe_and_interact(websocket)

async def get_token():
    import httpx
    async with httpx.AsyncClient() as client:
        response = await client.post("http://localhost:8000/token")
        response.raise_for_status()
        data = response.json()
        return data['access_token']

async def subscribe_and_interact(websocket):
    # Suscribirse a topic1
    await websocket.send(json.dumps({"topic": "topic2", "content": "Subscribing to topic1"}))
    print("Subscribed to topic1")

    # Función para recibir mensajes en segundo plano
    async def receive_messages():
        try:
            while True:
                message = await websocket.recv()
                print(f"\nReceived message on topic2: {message}")
        except websockets.ConnectionClosed:
            print("Connection closed")

    # Iniciar la tarea de recepción de mensajes
    receive_task = asyncio.create_task(receive_messages())

    try:
        while True:
            # Leer mensaje de la entrada estándar (consola)
            message = await asyncio.get_event_loop().run_in_executor(None, input, "Enter message to send to topic1: ")
            # Enviar el mensaje al WebSocket
            await websocket.send(json.dumps({"topic": "topic2", "content": message}))
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        receive_task.cancel()
        await websocket.close()

if __name__ == "__main__":
    asyncio.run(authenticate_and_connect())
