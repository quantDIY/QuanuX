class Strategy:
    async def on_start(self, adapter):
        print(f"Strategy Started on adapter: {adapter.name}")
        await adapter.place_order({"symbol": "TEST", "side": "BUY"})
