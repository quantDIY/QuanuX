from server.strategies.base import EntryModule

class CustomEntry(EntryModule):
    def generate_entry(self, signal, bar_data):
        return None