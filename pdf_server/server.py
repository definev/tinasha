#!/usr/bin/env python
import json
from typing import List

from fastapi import FastAPI
from langchain.prompts import ChatPromptTemplate

from langchain.chat_models import ChatOpenAI
from langchain.embeddings import GooglePalmEmbeddings
from langchain.chat_models import ChatGooglePalm
import google.generativeai as palm

from langchain.schema import BaseOutputParser
from langserve import add_routes

with open("credentials.json", "r") as f:
    cred = json.load(f)
    api_key = cred.get("google_api_key")

class CommaSeparatedListOutputParser(BaseOutputParser[List[str]]):
    """Parse the output of an LLM call to a comma-separated list."""


    def parse(self, text: str) -> List[str]:
        """Parse the output of an LLM call."""
        return text.strip().split(", ")

template = """You are a helpful assistant who generates comma separated lists.
A user will pass in a category, and you should generate 5 objects in that category in a comma separated list.
ONLY return a comma separated list, and nothing more."""
human_template = "{text}"

chat_prompt = ChatPromptTemplate.from_messages([
    ("system", template),
    ("human", human_template),
])
category_chain = chat_prompt | ChatGooglePalm(google_api_key=api_key) | CommaSeparatedListOutputParser()

# 2. App definition
app = FastAPI(
  title="LangChain Server",
  version="1.0",
  description="A simple api server using Langchain's Runnable interfaces",
)

# 3. Adding chain route
add_routes(
    app,
    category_chain,
    path="/category_chain",
)

if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="localhost", port=8000)