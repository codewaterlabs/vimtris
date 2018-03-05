let sdfDist = (cols, rows, tileSpace) => {
  let colsGl = string_of_float(cols);
  let rowsGl = string_of_float(rows);
  {|
        float cols = |}
  ++ colsGl
  ++ {|;
        float rows = |}
  ++ rowsGl
  ++ {|;
        float cols2 = cols * 2.0;
        float rows2 = rows * 2.0;
        point.x = mod(point.x, 1.0 / cols) - 1.0 / cols2;
        point.y = mod(point.y, 1.0 / rows) - 1.0 / rows2;
        float boxWidth = 1.0 / cols2;
        float boxHeight = 1.0 / rows2;
        float boxDepth = 0.005;
        float box = length(max(abs(point) - vec3(boxWidth, boxHeight, boxDepth), vec3(0.0, 0.0, 0.0)));
        // Octahedron towards z
        /*
        float rot = 20.0;
        mat3 xrot = mat3(
            1.0, 0.0, 0.0,
            0.0, cos(rot), -sin(rot),
            0.0, sin(rot), cos(rot)
        );
        point = point * xrot;
        */
        float d = 0.0;
        // Dont know too much of what I'm doing here..
        // sdpyramid from https://www.shadertoy.com/view/Xds3zN with some modifications
        vec3 octa = vec3(0.5 * boxHeight / boxWidth, 0.5, 0.24);
        d = max( d, abs( dot(point, vec3( -octa.x, 0, octa.z )) ));
        d = max( d, abs( dot(point, vec3(  octa.x, 0, octa.z )) ));
        d = max( d, abs( dot(point, vec3(  0, -octa.y, octa.z )) ));
        d = max( d, abs( dot(point, vec3(  0, octa.y, octa.z )) ));
        // Some spacing added, maybe this should be calibrated to a pixel
        float o = d - octa.z / (rows + |}
  ++ string_of_float(tileSpace)
  ++ {|);
        // Intersection
        //return o;
        return max(o, box);
    |};
};

type sdfTiles = {
  cols: float,
  rows: float,
  tileSpace: float,
  sdfNode: SdfNode.t
};

module StoreSpec = {
  type hash = {
    hCols: float,
    hRows: float,
    hTileSpace: float,
    hSdfNode: SdfNode.hash
  };
  type progType = sdfTiles;
  let getHash = sdfTiles => {
    hCols: sdfTiles.cols,
    hRows: sdfTiles.rows,
    hTileSpace: sdfTiles.tileSpace,
    hSdfNode: SdfNode.makeHash(sdfTiles.sdfNode)
  };
  let createProgram = sdfTiles => SdfNode.makeProgram(sdfTiles.sdfNode);
  let tblSize = 3;
};

module Programs = ProgramStore.Make(StoreSpec);

let makeNode =
    (
      cols,
      rows,
      lighting,
      ~drawTo=?,
      ~vo=?,
      ~color=?,
      ~key=?,
      ~model=?,
      ~margin=?,
      ~tileSpace=1.3,
      ()
    ) => {
  let aspect = cols /. rows;
  let fragCoords =
    switch model {
    | Some(_) => SdfNode.ByModel
    | None => SdfNode.ZeroToOne
    };
  let sdfNode =
    SdfNode.make(
      sdfDist(cols, rows, tileSpace),
      fragCoords,
      model,
      lighting,
      ~vo?,
      ~color?,
      ()
    );
  let sdfTiles: sdfTiles = {cols, rows, tileSpace, sdfNode};
  let program = Programs.getProgram(sdfTiles);
  SdfNode.makeNode(
    sdfNode,
    ~program,
    ~key?,
    ~cls="sdfTiles",
    ~aspect,
    ~drawTo?,
    ~margin?,
    ()
  );
};
