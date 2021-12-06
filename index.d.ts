
interface Param {
    buffer: Uint8Array
    start: number
    end: number
    pos: number
}

export class Compression {
    static readonly recommendedInputBufferSize: number
    static readonly recommendedOutputBufferSize: number
    compress(input: Param, output: Param): number
    end(input: Param, output: Param): number
}

export class Decompression {
    static readonly recommendedInputBufferSize: number
    static readonly recommendedOutputBufferSize: number
    decompress(input: Param, output: Param): number
}
