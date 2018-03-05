type t = {
  points: list((int, int)),
  points90: list((int, int)),
  points180: list((int, int)),
  points270: list((int, int)),
  colorIndex: int
};

let make = (points, (translateX, translateY), colorIndex) => {
  let rotate90 = ((x, y)) => (y, x * (-1));
  let points90 = List.map(rotate90, points);
  let points180 = List.map(rotate90, points90);
  let points270 = List.map(rotate90, points180);
  /* Translate and set upper left corner points */
  let toTilePoints = ((x, y)) => (
    /* Not sure about why y is flipped.. */
    (x - translateX) / 2 - 1,
    (y - translateY) / (-2) - 1
  );
  {
    points: List.map(toTilePoints, points),
    points90: List.map(toTilePoints, points90),
    points180: List.map(toTilePoints, points180),
    points270: List.map(toTilePoints, points270),
    colorIndex
  };
};

/* Origin is in x2 to simplify to ints */
let lineTiles = make([((-3), 1), ((-1), 1), (1, 1), (3, 1)], (1, 1), 2);

let leftLTiles = make([((-2), 2), ((-2), 0), (0, 0), (2, 0)], (0, 0), 3);

let rightLTiles = make([((-2), 0), (0, 0), (2, 0), (2, 2)], (0, 0), 4);

let cubeTiles = make([((-1), 1), ((-1), (-1)), (1, 1), (1, (-1))], (1, 1), 5);

let rightTurnTiles = make([((-2), 0), (0, 0), (0, 2), (2, 2)], (0, 0), 6);

let triangleTiles = make([((-2), 0), (0, 0), (0, 2), (2, 0)], (0, 0), 7);

let leftTurnTiles = make([((-2), 2), (0, 2), (0, 0), (2, 0)], (0, 0), 8);
