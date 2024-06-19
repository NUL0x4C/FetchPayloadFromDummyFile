## FetchPayloadFromDummyFile: Construct a payload at runtime using an array of offsets

## What Is This?

This is simply another technique to obfuscate the payload while simultaneously lowering its entropy.

## How Does It Work?

Using the [OffsetArrayBuilder](https://github.com/NUL0x4C/FetchPayloadFromDummyFile/tree/main/OffsetArrayBuilder) program, one can create an array of `DWORD` values, where each element in this array represents an offset of where a payload's byte is located in a dummy file specified by the user. For example, if we go to the `0x30A` offset (`778` in decimal) in the `dummygif.gif` file. We'll find the first byte of our payload (`0xFC`).

![image](https://github.com/NUL0x4C/FetchPayloadFromDummyFile/assets/111295429/bb90a6a2-157b-4a04-b418-267d9d27a463)

The PoC reads the [dummygif.fig](https://github.com/NUL0x4C/FetchPayloadFromDummyFile/blob/main/dummygif.gif) file and searches for matching bytes with the payload, saving the indexes of where the elements matched in an array. In the [execution implementation](https://github.com/NUL0x4C/FetchPayloadFromDummyFile/blob/main/FetchPayloadFromDummyFile/main.c), you only need the offset array and the same dummy file. 

## Disadvantages

Instead of storing the payload in your implementation, you will need to store the generated offsets array that will be of a size equal to `sizeof(payload) * sizeof(DWORD)`, which is 4 times the size of the payload. In addition to this, you'll need the dummy file used by the `OffsetArrayBuilder.exe`. To solve the latter issue, one can use a built-in Windows file as the dummy file instead of bringing his file. 


## Learn More About Malware Development

[Maldev Academy Home](https://maldevacademy.com?ref=gh)
  
[Maldev Academy Syllabus](https://maldevacademy.com/syllabus?ref=gh)

[Maldev Academy Pricing](https://maldevacademy.com/pricing?ref=gh)
