from pydantic import BaseModel

class TokenData(BaseModel):
    username: str

class Message(BaseModel):
    topic: str
    content: str
