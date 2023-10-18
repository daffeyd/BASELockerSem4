import uvicorn
import pickle
from fastapi import FastAPI
import pandas as pd
from locker import Locker
import joblib
import numpy as np
# init app
app = FastAPI()


returnModel = joblib.load("models\rf_return_model.pkl")
recommendationModel = joblib.load("models\rf_recommendation_model.pkl")


# Define a dictionary to relabel the predicted class labels
label_map = {'Friday': 0, 'Monday': 1, 'Thursday': 2, 'Tuesday': 3, 'Wednesday': 4}
# routes


@app.get('/')
async def index():
    return {"text": "Hello this is machine learning API that created by Daffeyd Wilbert"}


@app.get('/items/{name}')
async def get_items(name):
    return {"name": name}


@app.post('/P_return')
async def predict_return_p(data: Locker ):
    data = data.dict()
    day = data['day']
    day = pd.Series(day).map(label_map)
    day = day.values[0]
    shift = data['shift']
    data_pred = np.array([day, shift]).reshape(1, -1)
    y_pred = returnModel.predict(data_pred)
    return {"prediction": str(y_pred[0])}

@app.post('/P_recommendation')
async def predict_recommendation_p(data: Locker ):
    data = data.dict()
    day = data['day']
    day = pd.Series(day).map(label_map)
    day = day.values[0]
    shift = data['shift']
    data_pred = np.array([day, shift]).reshape(1, -1)
    y_pred = recommendationModel.predict(data_pred)
    return {"prediction": str(y_pred[0])}

@app.post('/G_return/{day}/{shift}')
async def predict_return_g(day,shift):
    day = day
    day = pd.Series(day).map(label_map)
    day = day.values[0]
    shift = int(shift)
    data_pred = np.array([day, shift]).reshape(1, -1)
    y_pred = returnModel.predict(data_pred)
    return {"prediction": str(y_pred[0])}

@app.post('/G_recommendation/{day}/{shift}')
async def predict_recommendation_g(day,shift):
    day = day
    day = pd.Series(day).map(label_map)
    day = day.values[0]
    shift = int(shift)
    data_pred = np.array([day, shift]).reshape(1, -1)
    y_pred = recommendationModel.predict(data_pred)
    return {"prediction": str(y_pred[0])}


if __name__ == '__main__':
    uvicorn.run(app, host='127.0.0.1    ', port=8000)

