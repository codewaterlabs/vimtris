let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 layout;
    varying vec2 vPosition;
    void main() {
        vPosition = position;
        vec3 pos2 = vec3(position, 1.0) * layout;
        gl_Position = vec4(pos2.xy, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    varying vec2 vPosition;

    void main() {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 0.6);
    }
|};

open Gpu;

let makeNode = vo =>
  Scene.makeNode(
    ~key="tileBlink",
    ~vertShader=Shader.make(vertexSource),
    ~fragShader=Shader.make(fragmentSource),
    ~transparent=true,
    ~hidden=true,
    ~vo,
    ()
  );
