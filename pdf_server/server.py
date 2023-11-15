#!/usr/bin/env python
import json

from fastapi import FastAPI
from langchain.prompts import PromptTemplate
from langchain.prompts.base import StringPromptValue

from langchain.embeddings import GooglePalmEmbeddings
from langchain.llms import GooglePalm
from langchain.text_splitter import CharacterTextSplitter
from langchain.vectorstores import FAISS

from langchain.chains.question_answering import load_qa_chain
from langserve import add_routes

from langchain.schema.runnable import RunnableLambda

# MODEL SETUP
with open("credentials.json", "r") as f:
    cred = json.load(f)
    api_key = cred.get("google_api_key")

defaults = {
    "model": "models/text-bison-001",
    "temperature": 1,
    "candidate_count": 1,
    "top_k": 40,
    "top_p": 0.95,
    "max_output_tokens": 1024,
    "stop_sequences": [],
    "safety_settings": [
        {"category": "HARM_CATEGORY_DEROGATORY", "threshold": 1},
        {"category": "HARM_CATEGORY_TOXICITY", "threshold": 1},
        {"category": "HARM_CATEGORY_VIOLENCE", "threshold": 2},
        {"category": "HARM_CATEGORY_SEXUAL", "threshold": 2},
        {"category": "HARM_CATEGORY_MEDICAL", "threshold": 2},
        {"category": "HARM_CATEGORY_DANGEROUS", "threshold": 2},
    ],
}

llm = GooglePalm(google_api_key=api_key, **defaults)
# ====================

# LOAD PDF

def process_text(text):
    # Split the text into chunks using Langchain's CharacterTextSplitter
    text_splitter = CharacterTextSplitter(
        separator="\n",
        chunk_size=1000,
        chunk_overlap=200,
        length_function=len
    )
    chunks = text_splitter.split_text(text)

    embeddings = GooglePalmEmbeddings(google_api_key=api_key)
    knowledgeBase = FAISS.from_texts(chunks, embeddings)

    return knowledgeBase

text = ""

knowledgeBase = None

chain = load_qa_chain(llm, chain_type="stuff")

def handle_question(question: StringPromptValue):
    question = question.to_string()
    if len(text) == 0:
        return "No document loaded yet. Please load a document first."
    
    docs = knowledgeBase.similarity_search(question)
    response = chain.run(input_documents=docs, question=question)

    return response

prompt_format = "Using the given document, answer the following question: {question}"

prompt = PromptTemplate(
    input_variables=["question"],
    template=prompt_format,
)

pdf_ask_chain = (
    prompt | RunnableLambda(handle_question)
)


app = FastAPI(
    title="Test PDF Server",
    version="1.0",
    description="A simple api server to chat with PDF",
)

# Ask PDF route
add_routes(
    app,
    pdf_ask_chain,
    path="/ask_pdf",
)

# Add PDF route
add_pdf_format = "{content}"

add_prompt = PromptTemplate(
    input_variables=["content"],
    template=add_pdf_format,
)

def add_pdf(content: StringPromptValue):
    global text
    text = content.to_string()
    global knowledgeBase
    knowledgeBase = process_text(text)
    return "PDF added"

pdf_add_chain = (
    add_prompt | RunnableLambda(add_pdf)
)

add_routes(
    app,
    pdf_add_chain,
    path="/add_pdf",
)

if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="localhost", port=8000)