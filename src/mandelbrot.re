let maxIterations = 99999;

type complexNumber = {
  real: float,
  imaginary: float
};

let multComplex = (c1, c2) => {
  real: c1.real *. c2.real -. c1.imaginary *. c2.imaginary,
  imaginary: c1.real *. c2.imaginary +. c1.imaginary *. c2.real
};

let addComplex = (c1, c2) => {
  real: c1.real +. c2.real,
  imaginary: c1.imaginary +. c2.imaginary
};

let absComplex = c =>
  Js_math.sqrt(c.real *. c.real +. c.imaginary *. c.imaginary);

let calcIterations = (x, y) => {
  let z = {real: 0., imaginary: 0.};
  let c = {real: x, imaginary: y};
  let rec iterate = (z, c, iterations) => {
    let z = addComplex(multComplex(z, z), c);
    if (absComplex(z) > 2.0) {
      iterations;
    } else {
      iterate(z, c, iterations + 1);
    };
  };
  iterate(z, c, 0);
};

module Constants = RGLConstants;

module Gl = Reasongl.Gl;

let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    varying vec2 vPosition;
    void main() {
        vPosition = position;
        gl_Position = vec4(position, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    varying vec2 vPosition;
    const int max_iterations = 80;

    float range_over(float range, int iterations) {
        return float(range - min(range, float(iterations))) / range;
    }

    void main() {
        vec2 z = vec2(0.0, 0.0);
        float z_r_temp = 0.0;
        vec2 c = vec2(vPosition.x * 1.4, vPosition.y * 1.2) * 0.15 + 0.4;
        int iterations = 0;
        for (int i = 0; i <= max_iterations; i++) {
            // z = z*z + c
            z_r_temp = (z.x*z.x - z.y*z.y) + c.x;
            z = vec2(z_r_temp, ((z.x*z.y) * 2.0) + c.y);
            if (length(z) > 2.0) {
                iterations = i;
                break;
            }
        }
        if (iterations == 0) {
            iterations = max_iterations;
        }
        float l_extra = length(z) - 2.0;
        vec3 color = vec3(
            range_over(4.0 + l_extra, iterations),
            range_over(12.0 + l_extra, iterations),
            range_over(80.0 + l_extra, iterations)
        );
        gl_FragColor = vec4(color, 1.0);
    }
|};

open Gpu;

let createCanvas = () => {
  let canvas = Canvas.init(400, 300);
  let drawState =
    DrawState.init(
      canvas.context,
      Program.make(
        Shader.make(vertexSource),
        Shader.make(fragmentSource),
        VertexBuffer.quadAttribs(),
        []
      ),
      VertexBuffer.makeQuad(),
      Some(IndexBuffer.makeQuad()),
      [],
      canvas.gpuState
    );
  DrawState.draw(drawState, canvas);
};
