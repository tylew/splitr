FROM arm64v8/python:3.6
RUN apt-get update && apt-get install -y bluez
RUN pip install pygame

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application code
COPY . .

# Command to run 
CMD ["python", "app.py"]
