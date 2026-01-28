from quart import Quart, render_template
import httpx
import asyncio
import json

app = Quart(__name__)

async def fetch(client, url):
    response = await client.get(url)
    return response.json()

@app.route("/")
async def index():
    url = "https://api.thingspeak.com/channels/3233855/feeds.json?api_key=E7PV777HU2V2R513"

    async with httpx.AsyncClient() as client:
        data = await fetch(client, url)

    return await render_template("index.html", data=data)

if __name__ == "__main__":
    app.run(debug=True, port=5000)
