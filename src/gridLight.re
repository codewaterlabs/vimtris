let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform mat3 layout;
    uniform mat3 texTrans;
    varying vec3 screenP;

    void main() {
        vec2 transformed = (vec3(position, 1.0) * layout).xy;
        screenP = vec3(transformed, 0.0);
        // Todo: Better support texTrans etc to avoid
        // manual bickering
        gl_Position = vec4((vec3(position, 1.0) * texTrans).xy, 0.0, 1.0);
    }
|};

let fragmentSource = (pointLight, camera) =>
  {|
    precision mediump float;
    varying vec3 screenP;

    |}
  ++ Light.PointLight.getLightFunction(pointLight, camera)
  ++ {|

    void main() {
        // Global pointLight (not using local position)
        vec3 pointLight = lighting(vec3(0.0), screenP, vec3(0.0, 0.0, 1.0));
        gl_FragColor = vec4(pointLight, 1.0);
    }
  |};

open Gpu;

let makeNode = () => {
  let pointLight =
    Light.PointLight.make(~pos=StaticPos(Config.pointLight), ~specular=24, ());
  let camera = Camera.make(Config.camera);
  Scene.(
    makeNode(
      ~key="gridLight",
      ~vertShader=Shader.make(vertexSource),
      ~fragShader=Shader.make(fragmentSource(pointLight, camera)),
      ~drawTo=TextureRGB,
      ~texTransUniform=true,
      ()
    )
  );
};
