from quart import Quart, render_template
import httpx
import asyncio
import json

app = Quart(__name__)

with open("secrets.json", "r") as file:
    secrets = json.load(file)

API_KEY = secrets["api_key"]

async def fetch(client, url):
    response = await client.get(url)
    return response.json()

@app.route("/")
async def index():
    url = f"https://api.thingspeak.com/channels/3233855/feeds.json?api_key={API_KEY}"

    async with httpx.AsyncClient() as client:
        data = await fetch(client, url)

    return await render_template("index.html", data=data)

if __name__ == "__main__":
    app.run(debug=True, port=5000)
