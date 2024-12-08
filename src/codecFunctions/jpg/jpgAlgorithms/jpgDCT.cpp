#include "jpgDCT.hpp"

#include <math.h>


// To do this parT I will use an optimization of this algorithm known as AAN

// This is based on the implementation in https://github.com/dannye/jed/tree/master

// DCT scaling factors
const float m0 = 2.0 * std::cos(1.0 / 16.0 * 2.0 * M_PI);
const float m1 = 2.0 * std::cos(2.0 / 16.0 * 2.0 * M_PI);
const float m3 = 2.0 * std::cos(2.0 / 16.0 * 2.0 * M_PI);
const float m5 = 2.0 * std::cos(3.0 / 16.0 * 2.0 * M_PI);
const float m2 = m0 - m5;
const float m4 = m0 + m5;

const float s0 = std::cos(0.0 / 16.0 * M_PI) / std::sqrt(8);
const float s1 = std::cos(1.0 / 16.0 * M_PI) / 2.0;
const float s2 = std::cos(2.0 / 16.0 * M_PI) / 2.0;
const float s3 = std::cos(3.0 / 16.0 * M_PI) / 2.0;
const float s4 = std::cos(4.0 / 16.0 * M_PI) / 2.0;
const float s5 = std::cos(5.0 / 16.0 * M_PI) / 2.0;
const float s6 = std::cos(6.0 / 16.0 * M_PI) / 2.0;
const float s7 = std::cos(7.0 / 16.0 * M_PI) / 2.0;


/// The operations in this function are based on inverting the graph in this part of the video
/// https://youtu.be/Ct39BdHC2fE?si=t3HplbTsP6Ak3x1u&t=939

void applyDctToBlock(JpgBlock & block){

    float intermediate[64];

    for(int i = 0; i<8; ++i){

        const float a0 = block.blockData[0*8 + i];
        const float a1 = block.blockData[1*8 + i];
        const float a2 = block.blockData[2*8 + i];
        const float a3 = block.blockData[3*8 + i];
        const float a4 = block.blockData[4*8 + i];
        const float a5 = block.blockData[5*8 + i];
        const float a6 = block.blockData[6*8 + i];
        const float a7 = block.blockData[7*8 + i];

        const float b0 = a0 + a7;
        const float b1 = a1 + a6;
        const float b2 = a2 + a5;
        const float b3 = a3 + a4;
        const float b4 = a3 - a4;
        const float b5 = a2 - a5;
        const float b6 = a1 - a6;
        const float b7 = a0 - a7;
        
        const float c0 = b0 + b3;
        const float c1 = b1 + b2;
        const float c2 = b1 - b2;
        const float c3 = b0 - b3;
        const float c4 = b4;
        const float c5 = b5 - b4;
        const float c6 = b6 - c5; // Be careful, here is c5 not b5
        const float c7 = b7 - b6;

        const float d0 = c0 + c1;
        const float d1 = c0 - c1;
        const float d2 = c2;
        const float d3 = c3 - c2;
        const float d4 = c4;
        const float d5 = c5;
        const float d6 = c6;
        const float d7 = c7 + c5;
        const float d8 = c4 - c6;

        const float e0 = d0;
        const float e1 = d1;
        const float e2 = d2*m1;
        const float e3 = d3;
        const float e4 = d4*m2;
        const float e5 = d5*m3;
        const float e6 = d6*m4;
        const float e7 = d7;
        const float e8 = d8*m5;
        
        const float f0 = e0;
        const float f1 = e1;
        const float f2 = e2 + e3;
        const float f3 = e3 - e2;
        const float f4 = e4 + e8;
        const float f5 = e5 + e7;
        const float f6 = e6 + e8;
        const float f7 = e7 - e5;
        
        const float g0 = f0;
        const float g1 = f1;
        const float g2 = f2;
        const float g3 = f3;
        const float g4 = f4 + f7;
        const float g5 = f5 + f6;
        const float g6 = f5 - f6;
        const float g7 = f7 - f4;

        intermediate[0*8 + i] = g0 * s0;
        intermediate[1*8 + i] = g5 * s1;
        intermediate[2*8 + i] = g2 * s2;
        intermediate[3*8 + i] = g7 * s3;
        intermediate[4*8 + i] = g1 * s4;
        intermediate[5*8 + i] = g4 * s5;
        intermediate[6*8 + i] = g3 * s6;
        intermediate[7*8 + i] = g6 * s7;
    }

    for(int i = 0; i<8; ++i){

        const float a0 = intermediate[i*8 + 0];
        const float a1 = intermediate[i*8 + 1];
        const float a2 = intermediate[i*8 + 2];
        const float a3 = intermediate[i*8 + 3];
        const float a4 = intermediate[i*8 + 4];
        const float a5 = intermediate[i*8 + 5];
        const float a6 = intermediate[i*8 + 6];
        const float a7 = intermediate[i*8 + 7];

        const float b0 = a0 + a7;
        const float b1 = a1 + a6;
        const float b2 = a2 + a5;
        const float b3 = a3 + a4;
        const float b4 = a3 - a4;
        const float b5 = a2 - a5;
        const float b6 = a1 - a6;
        const float b7 = a0 - a7;
        
        const float c0 = b0 + b3;
        const float c1 = b1 + b2;
        const float c2 = b1 - b2;
        const float c3 = b0 - b3;
        const float c4 = b4;
        const float c5 = b5 - b4;
        const float c6 = b6 - c5; // Be careful, here is c5 not b5
        const float c7 = b7 - b6;

        const float d0 = c0 + c1;
        const float d1 = c0 - c1;
        const float d2 = c2;
        const float d3 = c3 - c2;
        const float d4 = c4;
        const float d5 = c5;
        const float d6 = c6;
        const float d7 = c7 + c5;
        const float d8 = c4 - c6;

        const float e0 = d0;
        const float e1 = d1;
        const float e2 = d2*m1;
        const float e3 = d3;
        const float e4 = d4*m2;
        const float e5 = d5*m3;
        const float e6 = d6*m4;
        const float e7 = d7;
        const float e8 = d8*m5;
        
        const float f0 = e0;
        const float f1 = e1;
        const float f2 = e2 + e3;
        const float f3 = e3 - e2;
        const float f4 = e4 + e8;
        const float f5 = e5 + e7;
        const float f6 = e6 + e8;
        const float f7 = e7 - e5;
        
        const float g0 = f0;
        const float g1 = f1;
        const float g2 = f2;
        const float g3 = f3;
        const float g4 = f4 + f7;
        const float g5 = f5 + f6;
        const float g6 = f5 - f6;
        const float g7 = f7 - f4;

        block.blockData[i*8 + 0] = g0 * s0;
        block.blockData[i*8 + 1] = g5 * s1;
        block.blockData[i*8 + 2] = g2 * s2;
        block.blockData[i*8 + 3] = g7 * s3;
        block.blockData[i*8 + 4] = g1 * s4;
        block.blockData[i*8 + 5] = g4 * s5;
        block.blockData[i*8 + 6] = g3 * s6;
        block.blockData[i*8 + 7] = g6 * s7;
    }
}


/// The operations in this function are based on the graph in this part of the video
/// https://youtu.be/Ct39BdHC2fE?si=t3HplbTsP6Ak3x1u&t=939

void inverseDCTBlock(JpgBlock & block){

    float intermediate[64];

    for (uint i = 0; i < 8; ++i) {
        const float g0 = block.blockData[0 * 8 + i] * s0;
        const float g1 = block.blockData[4 * 8 + i] * s4;
        const float g2 = block.blockData[2 * 8 + i] * s2;
        const float g3 = block.blockData[6 * 8 + i] * s6;
        const float g4 = block.blockData[5 * 8 + i] * s5;
        const float g5 = block.blockData[1 * 8 + i] * s1;
        const float g6 = block.blockData[7 * 8 + i] * s7;
        const float g7 = block.blockData[3 * 8 + i] * s3;

        const float f0 = g0; /// This constants are only to match the map, the compiler will delete them
        const float f1 = g1;
        const float f2 = g2;
        const float f3 = g3;
        const float f4 = g4 - g7;
        const float f5 = g5 + g6;
        const float f6 = g5 - g6;
        const float f7 = g4 + g7;

        const float e0 = f0;
        const float e1 = f1;
        const float e2 = f2 - f3;
        const float e3 = f2 + f3;
        const float e4 = f4;
        const float e5 = f5 - f7;
        const float e6 = f6;
        const float e7 = f5 + f7;
        const float e8 = f4 + f6;

        const float d0 = e0;
        const float d1 = e1;
        const float d2 = e2 * m1;
        const float d3 = e3;
        const float d4 = e4 * m2;
        const float d5 = e5 * m3;
        const float d6 = e6 * m4;
        const float d7 = e7;
        const float d8 = e8 * m5;

        const float c0 = d0 + d1;
        const float c1 = d0 - d1;
        const float c2 = d2 - d3;
        const float c3 = d3;
        const float c4 = d4 + d8;
        const float c5 = d5 + d7;
        const float c6 = d6 - d8;
        const float c7 = d7;
        const float c8 = c5 - c6;

        const float b0 = c0 + c3;
        const float b1 = c1 + c2;
        const float b2 = c1 - c2;
        const float b3 = c0 - c3;
        const float b4 = c4 - c8;
        const float b5 = c8;
        const float b6 = c6 - c7;
        const float b7 = c7;

        intermediate[0 * 8 + i] = b0 + b7;
        intermediate[1 * 8 + i] = b1 + b6;
        intermediate[2 * 8 + i] = b2 + b5;
        intermediate[3 * 8 + i] = b3 + b4;
        intermediate[4 * 8 + i] = b3 - b4;
        intermediate[5 * 8 + i] = b2 - b5;
        intermediate[6 * 8 + i] = b1 - b6;
        intermediate[7 * 8 + i] = b0 - b7;
    }
    for (uint i = 0; i < 8; ++i) {
        const float g0 = intermediate[i * 8 + 0] * s0;
        const float g1 = intermediate[i * 8 + 4] * s4;
        const float g2 = intermediate[i * 8 + 2] * s2;
        const float g3 = intermediate[i * 8 + 6] * s6;
        const float g4 = intermediate[i * 8 + 5] * s5;
        const float g5 = intermediate[i * 8 + 1] * s1;
        const float g6 = intermediate[i * 8 + 7] * s7;
        const float g7 = intermediate[i * 8 + 3] * s3;

        const float f0 = g0;
        const float f1 = g1;
        const float f2 = g2;
        const float f3 = g3;
        const float f4 = g4 - g7;
        const float f5 = g5 + g6;
        const float f6 = g5 - g6;
        const float f7 = g4 + g7;

        const float e0 = f0;
        const float e1 = f1;
        const float e2 = f2 - f3;
        const float e3 = f2 + f3;
        const float e4 = f4;
        const float e5 = f5 - f7;
        const float e6 = f6;
        const float e7 = f5 + f7;
        const float e8 = f4 + f6;

        const float d0 = e0;
        const float d1 = e1;
        const float d2 = e2 * m1;
        const float d3 = e3;
        const float d4 = e4 * m2;
        const float d5 = e5 * m3;
        const float d6 = e6 * m4;
        const float d7 = e7;
        const float d8 = e8 * m5;

        const float c0 = d0 + d1;
        const float c1 = d0 - d1;
        const float c2 = d2 - d3;
        const float c3 = d3;
        const float c4 = d4 + d8;
        const float c5 = d5 + d7;
        const float c6 = d6 - d8;
        const float c7 = d7;
        const float c8 = c5 - c6;

        const float b0 = c0 + c3;
        const float b1 = c1 + c2;
        const float b2 = c1 - c2;
        const float b3 = c0 - c3;
        const float b4 = c4 - c8;
        const float b5 = c8;
        const float b6 = c6 - c7;
        const float b7 = c7;

        block.blockData[i * 8 + 0] = b0 + b7 + 0.5f;
        block.blockData[i * 8 + 1] = b1 + b6 + 0.5f;
        block.blockData[i * 8 + 2] = b2 + b5 + 0.5f;
        block.blockData[i * 8 + 3] = b3 + b4 + 0.5f;
        block.blockData[i * 8 + 4] = b3 - b4 + 0.5f;
        block.blockData[i * 8 + 5] = b2 - b5 + 0.5f;
        block.blockData[i * 8 + 6] = b1 - b6 + 0.5f;
        block.blockData[i * 8 + 7] = b0 - b7 + 0.5f;
    }
}

void applyDCT(std::vector<JpgBlock> & blocks){

    for(auto &block : blocks){
        applyDctToBlock(block);
    }

}


void inverseDCT(std::vector<JpgBlock> & blocks){

    for(auto &block : blocks){
        inverseDCTBlock(block);
    }

}