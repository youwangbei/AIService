import grpc
import warnings

import rag_pb2 as rag__pb2

GRPC_GENERATED_VERSION = '1.81.0'
GRPC_VERSION = grpc.__version__
_version_not_supported = False

try:
    from grpc._utilities import first_version_is_lower
    _version_not_supported = first_version_is_lower(GRPC_VERSION, GRPC_GENERATED_VERSION)
except ImportError:
    _version_not_supported = True

if _version_not_supported:
    raise RuntimeError(
        f'The grpc package installed is at version {GRPC_VERSION},'
        + ' but the generated code in rag_pb2_grpc.py depends on'
        + f' grpcio>={GRPC_GENERATED_VERSION}.'
        + f' Please upgrade your grpc module to grpcio>={GRPC_GENERATED_VERSION}'
        + f' or downgrade your generated code using grpcio-tools<={GRPC_VERSION}.'
    )


class RAGServiceStub:

    def __init__(self, channel):
        self.Query = channel.unary_unary(
                '/rag.RAGService/Query',
                request_serializer=rag__pb2.QueryRequest.SerializeToString,
                response_deserializer=rag__pb2.QueryResponse.FromString,
                _registered_method=True)
        self.AddDocument = channel.unary_unary(
                '/rag.RAGService/AddDocument',
                request_serializer=rag__pb2.AddDocumentRequest.SerializeToString,
                response_deserializer=rag__pb2.CommonResponse.FromString,
                _registered_method=True)
        self.Count = channel.unary_unary(
                '/rag.RAGService/Count',
                request_serializer=rag__pb2.CountRequest.SerializeToString,
                response_deserializer=rag__pb2.CountResponse.FromString,
                _registered_method=True)
        self.Clear = channel.unary_unary(
                '/rag.RAGService/Clear',
                request_serializer=rag__pb2.ClearRequest.SerializeToString,
                response_deserializer=rag__pb2.CommonResponse.FromString,
                _registered_method=True)


class RAGServiceServicer:

    def Query(self, request, context):
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def AddDocument(self, request, context):
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Count(self, request, context):
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Clear(self, request, context):
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_RAGServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'Query': grpc.unary_unary_rpc_method_handler(
                    servicer.Query,
                    request_deserializer=rag__pb2.QueryRequest.FromString,
                    response_serializer=rag__pb2.QueryResponse.SerializeToString,
            ),
            'AddDocument': grpc.unary_unary_rpc_method_handler(
                    servicer.AddDocument,
                    request_deserializer=rag__pb2.AddDocumentRequest.FromString,
                    response_serializer=rag__pb2.CommonResponse.SerializeToString,
            ),
            'Count': grpc.unary_unary_rpc_method_handler(
                    servicer.Count,
                    request_deserializer=rag__pb2.CountRequest.FromString,
                    response_serializer=rag__pb2.CountResponse.SerializeToString,
            ),
            'Clear': grpc.unary_unary_rpc_method_handler(
                    servicer.Clear,
                    request_deserializer=rag__pb2.ClearRequest.FromString,
                    response_serializer=rag__pb2.CommonResponse.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'rag.RAGService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))
    server.add_registered_method_handlers('rag.RAGService', rpc_method_handlers)


class RAGService:

    @staticmethod
    def Query(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/rag.RAGService/Query',
            rag__pb2.QueryRequest.SerializeToString,
            rag__pb2.QueryResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def AddDocument(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/rag.RAGService/AddDocument',
            rag__pb2.AddDocumentRequest.SerializeToString,
            rag__pb2.CommonResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def Count(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/rag.RAGService/Count',
            rag__pb2.CountRequest.SerializeToString,
            rag__pb2.CountResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def Clear(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/rag.RAGService/Clear',
            rag__pb2.ClearRequest.SerializeToString,
            rag__pb2.CommonResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)
