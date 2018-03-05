let vertexSource = {|
    precision mediump float;
    attribute vec2 position;
    uniform vec2 pixelSize;
    varying vec2 vPosition;
    varying vec2 pixelPos;
    void main() {
        float aspect = pixelSize.x / pixelSize.y;
        vPosition = (aspect > 1.0) ? vec2(position.x, position.y / aspect) : vec2(position.x * aspect, position.y);
        vPosition = vec2(position.x * aspect, position.y);
        // Normalize to 0.0 to 1.0
        //vPosition = vec2((vPosition.x + 1.0) * 0.5, (vPosition.y + 1.0) * 0.5);
        pixelPos = (position + vec2(1.0, -1.0)) * vec2(0.5, -0.5) * pixelSize;
        gl_Position = vec4(position, 0.0, 1.0);
    }
|};

/* http://alex-charlton.com/posts/Dithering_on_the_GPU/ */
let fragmentSource = {|
    precision mediump float;
    uniform float anim;
    uniform vec3 color;
    varying vec2 vPosition;
    varying vec2 pixelPos;

    const mat4 indexMat = mat4(
        0.0, 8.0, 2.0, 10.0,
        12.0, 4.0, 14.0, 6.0,
        3.0, 11.0, 1.0, 9.0,
        15.0, 7.0, 13.0, 5.0
    );

    const float colorInterval = 0.04;

    float indexVal() {
        int xMod = int(mod(pixelPos.x * 0.5, 4.0));
        int yMod = int(mod(pixelPos.y * 0.5, 4.0));
        float idxVal =
            (xMod == 0) ?
                (yMod == 0) ? indexMat[0][0]
                : (yMod == 1) ? indexMat[0][1]
                : (yMod == 2) ? indexMat[0][2]
                : indexMat[0][3]
            : (xMod == 1) ?
                (yMod == 0) ? indexMat[1][0]
                : (yMod == 1) ? indexMat[1][1]
                : (yMod == 2) ? indexMat[1][2]
                : indexMat[1][3]
            : (xMod == 2) ?
                (yMod == 0) ? indexMat[2][0]
                : (yMod == 1) ? indexMat[2][1]
                : (yMod == 2) ? indexMat[2][2]
                : indexMat[2][3]
            :
                (yMod == 0) ? indexMat[3][0]
                : (yMod == 1) ? indexMat[3][1]
                : (yMod == 2) ? indexMat[3][2]
                : indexMat[3][3];
        return idxVal / 16.0;
    }

    void main() {
        // Index value for this pixel
        float idxVal = indexVal();
        vec2 lightPos = vec2(0.0, 0.75);
        float dist = distance(lightPos, vPosition * vec2(1.0, 1.6)) / 2.5;
        // Distance in increasing power outwards
        float colorCoef = max(1.0 - (pow(4.0, dist) - 1.0) / 6.0, 0.0);
        float cMod = mod(colorCoef, colorInterval);
        float diffDown = cMod / colorInterval;
        bool isClosestDown = (diffDown < 0.5) ? true : false;
        float diff = (isClosestDown) ? diffDown : (1.0 - diffDown);
        bool useClosest = (diff < idxVal);
        float closestDown = colorCoef - cMod;
        float closestUp = colorCoef + colorInterval - cMod;
        colorCoef = useClosest ?
            (isClosestDown ? closestDown : closestUp)
            : (isClosestDown ? closestUp : closestDown);
        vec3 c = mix(vec3(0.0, 0.0, 0.0), color, colorCoef);
        gl_FragColor = vec4(c * anim, 1.0);
    }
|};

open Gpu;

let makeNode = (color, children) =>
  Scene.(
    Scene.makeNode(
      ~key="background",
      ~vertShader=Shader.make(vertexSource),
      ~fragShader=Shader.make(fragmentSource),
      ~uniforms=[("color", color), ("anim", UFloat.make(0.0))],
      ~pixelSizeUniform=true,
      ~size=Dimensions(Scale(1.0), Scale(1.0)),
      ~padding=Scale(0.05),
      ~vAlign=Scene.AlignMiddle,
      ~children,
      ()
    )
  );
