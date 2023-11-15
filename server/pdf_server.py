import requests

class PDFServerQuery:
    def __init__(self, config) -> None:
        self.pdf_server_url = config["pdf_server"]["url"]

    def add_pdf(self, content: str) -> str:
        request_body = {"input": {"content": content}}
        add_url = f"{self.pdf_server_url}/add_pdf/invoke"

        response = requests.post(add_url, json=request_body)
        return response.text
    
    def ask_pdf(self, question: str) -> str:
        request_body = {"input": {"question": question}}
        ask_url = f"{self.pdf_server_url}/ask_pdf/invoke"

        response = requests.post(ask_url, json=request_body)
        return response.json()["output"]
