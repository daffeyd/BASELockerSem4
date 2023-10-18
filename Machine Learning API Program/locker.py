from pydantic import BaseModel
# 2. Class which describes Bank Notes measurements
class Locker (BaseModel):
    day : str
    shift : float 