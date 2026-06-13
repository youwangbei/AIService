#!/usr/bin/env python3
import sys
import json
import re
import os
import glob
import shutil
import chromadb
from chromadb.config import Settings
from sentence_transformers import SentenceTransformer

def split_document(content, chunk_size=500, chunk_overlap=50):
    sentence_pattern = r'[^。！？.!?]+[。！？.!?]+'
    sentences = re.findall(sentence_pattern, content.strip())
    remaining = re.sub(sentence_pattern, '', content.strip()).strip()
    if remaining:
        sentences.append(remaining)
    
    chunks = []
    current_chunk = ""
    
    for sentence in sentences:
        sentence = sentence.strip()
        if not sentence:
            continue
        
        if not current_chunk:
            current_chunk = sentence
        elif len(current_chunk) + len(sentence) <= chunk_size:
            current_chunk += sentence
        else:
            if current_chunk:
                chunks.append(current_chunk)
            current_chunk = sentence
    
    if current_chunk:
        chunks.append(current_chunk)
    
    return chunks

def load_documents(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return f.read()
    except Exception as e:
        print(f"[ERROR] Failed to load document: {e}", file=sys.stderr)
        return None

class ChromaDBService:
    def __init__(self):
        self.model = SentenceTransformer("./m3e-small")
        self.running = True
        
        db_path = "./chroma_db"
        if os.path.exists(db_path):
            shutil.rmtree(db_path)
            print(f"[INFO] Removed existing database directory: {db_path}", file=sys.stderr)
        
        self.client = chromadb.PersistentClient(
            path="./chroma_db",
            settings=Settings(anonymized_telemetry=False)
        )
        
        self.collection = self.client.get_or_create_collection("knowledge_base")
        
        self.auto_preprocess()
    
    def auto_preprocess(self):
        doc_folder = "doc"
        supported_extensions = ('.md', '.txt', '.json', '.csv')
        
        if os.path.isdir(doc_folder):
            files = []
            for ext in supported_extensions:
                files.extend(glob.glob(os.path.join(doc_folder, f"*{ext}")))
            
            if files:
                print(f"[INFO] Found {len(files)} document(s) in {doc_folder}/", file=sys.stderr)
                
                total_chunks = 0
                for filepath in files:
                    filename = os.path.basename(filepath)
                    print(f"[INFO] Processing file: {filename}", file=sys.stderr)
                    
                    content = load_documents(filepath)
                    if content:
                        chunks = split_document(content)
                        print(f"[INFO]   Split into {len(chunks)} chunks", file=sys.stderr)
                        
                        for i, chunk in enumerate(chunks):
                            file_prefix = os.path.splitext(filename)[0]
                            doc_id = f"{file_prefix}_{i:04d}"
                            result = self.add_document(chunk, doc_id)
                            if result["status"] == "success":
                                total_chunks += 1
                            else:
                                print(f"[ERROR]   Failed to add chunk {i+1}: {result['message']}", file=sys.stderr)
                
                print(f"[INFO] Auto-preprocessing complete! Total chunks added: {total_chunks}", file=sys.stderr)
            else:
                print(f"[WARNING] No supported documents found in {doc_folder}/", file=sys.stderr)
        else:
            print(f"[WARNING] {doc_folder}/ directory not found", file=sys.stderr)
    
    def add_document(self, content, doc_id):
        try:
            embedding = self.model.encode(content).tolist()
            self.collection.add(
                documents=[content],
                embeddings=[embedding],
                ids=[doc_id]
            )
            return {"status": "success", "id": doc_id}
        except Exception as e:
            return {"status": "error", "message": str(e)}
    
    def query(self, query_text, top_k=3):
        try:
            query_embedding = self.model.encode(query_text).tolist()
            results = self.collection.query(
                query_embeddings=[query_embedding],
                n_results=top_k
            )
            
            documents = results['documents'][0] if results['documents'] else []
            ids = results['ids'][0] if results['ids'] else []
            distances = results['distances'][0] if results.get('distances') else []
            
            scores = [1.0 - d for d in distances] if distances else []
            
            return {
                "status": "success",
                "documents": documents,
                "sources": ids,
                "scores": scores
            }
        except Exception as e:
            return {"status": "error", "message": str(e)}
    
    def count(self):
        try:
            return {"status": "success", "count": self.collection.count()}
        except Exception as e:
            return {"status": "error", "message": str(e)}
    
    def clear(self):
        try:
            self.client.delete_collection("knowledge_base")
            self.collection = self.client.get_or_create_collection("knowledge_base")
            
            db_path = "./chroma_db"
            if os.path.exists(db_path):
                shutil.rmtree(db_path)
                print(f"[INFO] Cleared database directory: {db_path}", file=sys.stderr)
            
            return {"status": "success"}
        except Exception as e:
            return {"status": "error", "message": str(e)}

def main():
    service = ChromaDBService()
    print("[INFO] Preprocessing complete. Exiting...", file=sys.stderr)

if __name__ == "__main__":
    main()