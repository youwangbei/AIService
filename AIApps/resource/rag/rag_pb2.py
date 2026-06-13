from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    6,
    33,
    5,
    '',
    'rag.proto'
)

_sym_db = _symbol_database.Default()


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\trag.proto\x12\x03rag\",\n\x0cQueryRequest\x12\r\n\x05query\x18\x01 \x01(\t\x12\r\n\x05top_k\x18\x02 \x01(\x05\"d\n\rQueryResponse\x12\x0e\n\x06status\x18\x01 \x01(\t\x12\x11\n\tdocuments\x18\x02 \x03(\t\x12\x0f\n\x07sources\x18\x03 \x03(\t\x12\x0e\n\x06scores\x18\x04 \x03(\x01\x12\x0f\n\x07message\x18\x05 \x01(\t\"5\n\x12\x41\x64\x64\x44ocumentRequest\x12\x0f\n\x07\x63ontent\x18\x01 \x01(\t\x12\x0e\n\x06\x64oc_id\x18\x02 \x01(\t\"\x0e\n\x0c\x43ountRequest\"?\n\rCountResponse\x12\x0e\n\x06status\x18\x01 \x01(\t\x12\r\n\x05\x63ount\x18\x02 \x01(\x05\x12\x0f\n\x07message\x18\x03 \x01(\t\"\x0e\n\x0c\x43learRequest\"1\n\x0e\x43ommonResponse\x12\x0e\n\x06status\x18\x01 \x01(\t\x12\x0f\n\x07message\x18\x02 \x01(\t2\xda\x01\n\nRAGService\x12.\n\x05Query\x12\x11.rag.QueryRequest\x1a\x12.rag.QueryResponse\x12;\n\x0b\x41\x64\x64\x44ocument\x12\x17.rag.AddDocumentRequest\x1a\x13.rag.CommonResponse\x12.\n\x05\x43ount\x12\x11.rag.CountRequest\x1a\x12.rag.CountResponse\x12/\n\x05\x43lear\x12\x11.rag.ClearRequest\x1a\x13.rag.CommonResponseB*\n\x0f\x63om.example.ragB\x0fRAGServiceProtoP\x01\xa2\x02\x03RAGb\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'rag_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  _globals['DESCRIPTOR']._loaded_options = None
  _globals['DESCRIPTOR']._serialized_options = b'\n\017com.example.ragB\017RAGServiceProtoP\001\242\002\003RAG'
  _globals['_QUERYREQUEST']._serialized_start=18
  _globals['_QUERYREQUEST']._serialized_end=62
  _globals['_QUERYRESPONSE']._serialized_start=64
  _globals['_QUERYRESPONSE']._serialized_end=164
  _globals['_ADDDOCUMENTREQUEST']._serialized_start=166
  _globals['_ADDDOCUMENTREQUEST']._serialized_end=219
  _globals['_COUNTREQUEST']._serialized_start=221
  _globals['_COUNTREQUEST']._serialized_end=235
  _globals['_COUNTRESPONSE']._serialized_start=237
  _globals['_COUNTRESPONSE']._serialized_end=300
  _globals['_CLEARREQUEST']._serialized_start=302
  _globals['_CLEARREQUEST']._serialized_end=316
  _globals['_COMMONRESPONSE']._serialized_start=318
  _globals['_COMMONRESPONSE']._serialized_end=367
  _globals['_RAGSERVICE']._serialized_start=370
  _globals['_RAGSERVICE']._serialized_end=588
