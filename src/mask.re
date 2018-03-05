let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
    }
|};

let fragmentSource = {|
    precision mediump float;
    uniform float anim;

    void main() {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.5 * anim);
    }
|};

let makeNode = () =>
  Scene.(
    Scene.makeNode(
      ~key="mask",
      ~vertShader=Gpu.Shader.make(vertexSource),
      ~fragShader=Gpu.Shader.make(fragmentSource),
      ~transparent=true,
      ~hidden=true,
      ~uniforms=[("anim", UFloat.make(0.0))],
      ~size=Dimensions(Scale(1.0), Scale(1.0)),
      ()
    )
  );
