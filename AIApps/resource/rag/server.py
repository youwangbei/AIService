#!/usr/bin/env python3
import grpc
from concurrent import futures
import time
import sys

sys.path.insert(0, '/home/yb/AIService/AIApps/resource/rag')

import rag_pb2
import rag_pb2_grpc

class ChromaDBService:
    def __init__(self):
        import chromadb
        from sentence_transformers import SentenceTransformer
        
        self.model = SentenceTransformer("./m3e-small")
        self.client = chromadb.PersistentClient(path="./chroma_db")
        self.collection = self.client.get_or_create_collection("knowledge_base")
    
    def query(self, query_text, top_k=3):
        query_embedding = self.model.encode(query_text).tolist()
        results = self.collection.query(
            query_embeddings=[query_embedding],
            n_results=top_k
        )
        
        documents = results.get("documents", [[]])[0]
        sources = results.get("metadatas", [[]])[0]
        scores = results.get("distances", [[]])[0]
        
        return {
            "status": "success",
            "documents": documents,
            "sources": [str(s) for s in sources],
            "scores": scores
        }
    
    def add_document(self, content, doc_id):
        embedding = self.model.encode(content).tolist()
        self.collection.add(
            documents=[content],
            ids=[doc_id],
            embeddings=[embedding]
        )
        return {"status": "success", "message": f"Document {doc_id} added"}
    
    def count(self):
        count = self.collection.count()
        return {"status": "success", "count": count}
    
    def clear(self):
        self.collection.delete(where={})
        return {"status": "success", "message": "All documents cleared"}

class RAGServiceServicer(rag_pb2_grpc.RAGServiceServicer):
    def __init__(self):
        self.chroma_service = ChromaDBService()
        print("[INFO] RAG gRPC service initialized", flush=True)
    
    def Query(self, request, context):
        print(f"[INFO] Received query: {request.query}", flush=True)
        result = self.chroma_service.query(request.query, request.top_k)
        return rag_pb2.QueryResponse(
            status=result["status"],
            documents=result.get("documents", []),
            sources=result.get("sources", []),
            scores=result.get("scores", []),
            message=result.get("message", "")
        )
    
    def AddDocument(self, request, context):
        print(f"[INFO] Adding document: {request.doc_id}", flush=True)
        result = self.chroma_service.add_document(request.content, request.doc_id)
        return rag_pb2.CommonResponse(
            status=result["status"],
            message=result.get("message", "")
        )
    
    def Count(self, request, context):
        print("[INFO] Counting documents", flush=True)
        result = self.chroma_service.count()
        count_value = result.get("count", 0) if isinstance(result.get("count"), int) else 0
        return rag_pb2.CountResponse(
            status=result["status"],
            count=count_value,
            message=result.get("message", "")
        )
    
    def Clear(self, request, context):
        print("[INFO] Clearing documents", flush=True)
        result = self.chroma_service.clear()
        return rag_pb2.CommonResponse(
            status=result["status"],
            message=result.get("message", "")
        )

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    rag_pb2_grpc.add_RAGServiceServicer_to_server(RAGServiceServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print("[INFO] gRPC server started on port 50051", flush=True)
    try:
        while True:
            time.sleep(86400)
    except KeyboardInterrupt:
        print("[INFO] Shutting down gRPC server", flush=True)
        server.stop(0)

if __name__ == '__main__':
    serve()