const { Compression, Decompression } = require('.')

const compression = new Compression()

var compress   = { buffer: Buffer.alloc(Compression.recommendedInputBufferSize),  start: 0, end: Compression.recommendedInputBufferSize,  pos: 0 }
var compressed = { buffer: Buffer.alloc(Compression.recommendedOutputBufferSize), start: 0, end: Compression.recommendedOutputBufferSize, pos: 0 }

compress.end = compress.buffer.write('Hello, World!')

compression.end(compress, compressed)

const decompression = new Decompression()

var decompress   = { buffer: Buffer.alloc(Decompression.recommendedInputBufferSize),  start: 0, end: Decompression.recommendedInputBufferSize,  pos: 0 }
var decompressed = { buffer: Buffer.alloc(Decompression.recommendedOutputBufferSize), start: 0, end: Decompression.recommendedOutputBufferSize, pos: 0 }

compressed.buffer.copy(decompress.buffer, 0, 0, compressed.pos)

decompression.decompress(decompress, decompressed)

console.log(decompress)
console.log(decompressed.buffer.toString('utf-8', 0, decompressed.pos))
