/**
 * Copyright Prologin 2016 - Alexandre Macabies
 *
 * Main player.
 */

Phaser.Game.prototype.showDebugHeader = () => null;

const gs = 20, baseZoneMargin = 1.5 * gs, spriteScale = gs / 32;
var ticksPerState = 11;

const Direction = {
  N: new Phaser.Point(0, -1),
  S: new Phaser.Point(0, 1),
  E: new Phaser.Point(1, 0),
  W: new Phaser.Point(-1, 0),
  _: new Phaser.Point(0, 0),
  get: (a, b) => {
    let deltaX = b.x - a.x, deltaY = b.y - a.y;
    for (let dir of Direction.ALL)
      if (deltaX === dir.x && deltaY === dir.y)
        return dir;
    return Direction._;
  },
  flip: (d) => {
    if (d === Direction._) return d;
    return d === Direction.N ? Direction.S : d === Direction.S ? Direction.N : d === Direction.W ? Direction.E : Direction.W;
  }
};
Direction.ALL = [Direction.N, Direction.E, Direction.S, Direction.W];
Direction.NAMES = ['N', 'E', 'S', 'W']; // order is important

const DisplayMode = {NORMAL: 0, HEATMAP: 1};
const Orientation = {H: 0, V: 1};
const Players = [{uiid: 'a', color: 0xff71b0}, {uiid: 'b', color: 0x8aff73}];

const TurnState = {
  TURN_BEGINS: 0,
  ACTIONS_DONE: 1,
  PLASMA_MOVED: 2,
  PULSARS_LOOTED: 3
};
const CellType = {
  VOID: 0,
  PIPE: 1,
  SUPER_PIPE: 2,
  DEBRIS: 3,
  PULSAR: 4,
  BASE: 5,
  NOPE: 6
};

var board, game, gameManager, network;
var players = [];
var displayMode = DisplayMode.NORMAL, shouldUpdateUi = false;
var turnState = TurnState.TURN_BEGINS, turn = 0, nextMap = [], nextPlayers = [];
var autoForward = true;

var bgLayer;
var playZoneLayer;
var baseBgLayer;
var baseLayer;
var pipeLayer;
var pipeEnergyLayer;
var pulsarLayer;
var effectLayer;
var debugLayer;
var mouseCell;
var mouseCellLayer;

class Board {
  constructor(n) {
    this.n = n;
    this.entities = {};
  }

  iterate(type = Entity, cb) {
    for (let coord in this.entities) {
      if (!this.entities.hasOwnProperty(coord))
        continue;
      let entity = this.entities[coord];
      if (type === undefined || entity instanceof type)
        cb(entity);
    }
  }

  set(cell) {
    console.assert(cell instanceof Entity);
    this.entities[[cell.p.x, cell.p.y]] = cell;
  }

  getC(c) {
    return this.get(c.x, c.y);
  }

  get(x, y) {
    let e = this.entities[[x, y]];
    if (!e)
      return new Void(new Phaser.Point(x, y));
    return e;
  }

  removeC(p) {
    this.remove(p.x, p.y);
  }

  remove(x, y) {
    let e = this.entities[[x, y]];
    if (!e)
      return;
    e.remove();
    delete this.entities[[x, y]];
  }

  isPlayableC(p) {
    return this.isPlayable(p.x, p.y);
  }

  isPlayable(x, y) {
    return x >= 1 && x <= this.n - 2 && y >= 1 && y <= this.n - 2;
  }
}

class Network {

  constructor(url) {
    this.ready = false;
    this.stateCache = [];

    this.socket = new WebSocket(url);

    this.socket.onopen = () => {
      this.send('hello');
    };

    this.socket.onerror = (e) => {
      console.error(e);
    };

    this.socket.onmessage = (msg) => {
      msg = JSON.parse(msg.data);
      console.log('ws >', msg);
      this.handle(msg);
    };
  }

  hello() {
    gameManager.showWaiting(true);
    this.send('ready');
  }

  send(cmd, data) {
    let msg = $.extend({c: cmd}, data);
    console.log('ws <', msg);
    this.socket.send(JSON.stringify(msg));
  }

  getTurnState(turn, cb) {
    // TODO: adapt for replay
    if (this.stateCache[turn]) {
      cb.call(this, this.stateCache[turn]);
    } else {
      this.send('ready');
    }
  }

  handle(msg) {
    if (msg.c === 'whatsup') {
      board = new Board(msg.size);
      gameManager = new GameManager(msg.size, msg.maxTurn);
      return;
    }

    if (msg.c === 'turn') {
      // minus one because fuck logic
      let state = msg.state, thisTurn = state.turn[0];

      if (!this.stateCache[thisTurn]) {
        this.stateCache[thisTurn] = state;
      }

      turnState = TurnState.TURN_BEGINS;
      nextMap = state.map;
      nextPlayers = [];

      for (let pid in state.players) {
        if (state.players.hasOwnProperty(pid)) {
          let player = state.players[pid];
          player.pid = parseInt(pid);
          nextPlayers.push(player);
        }
      }
      nextPlayers.sort((a, b) => a.pid - b.pid);

      state.pulsars.map((data) => {
        let x = data.x, y = data.y, cell = board.get(x, y);
        if (!(cell instanceof Pulsar)) {
          board.remove(x, y);
          cell = new Pulsar(new Phaser.Point(x, y));
          board.set(cell);
        }
        cell.period = data.period;
        cell.energy = data.power;
        cell.stock = data.n_pulses;
      });

      if (turn === 0) {
        players = nextPlayers;
        gameManager.connected();
      }

      turn = thisTurn;
      if (turn >= gameManager.maxTurn) {
        turn = gameManager.maxTurn;
      }
      this.ready = true;
      gameManager.updateUi();
      gameManager.showWaiting(false);

      if (turn < gameManager.maxTurn && (autoForward || game.input.keyboard.isDown(Phaser.Keyboard.SPACEBAR)))
        gameManager.moveForward();

      return;
    }
  }
}

function sceneCoordToCell(p) {
  return new Phaser.Point(Math.floor((p.x - baseZoneMargin) / gs), Math.floor((p.y - baseZoneMargin) / gs));
}

function updateCellNeigh(pos) {
  for (let dir of Direction.ALL) {
    let neigh = board.get(pos.x + dir.x, pos.y + dir.y);
    if (!(neigh instanceof Void))
      neigh.update();
  }
}

function buildPipeGraph() {
  board.iterate(Pipe, pipe => {
    pipe.baseDistance = Infinity;
    pipe.cluster = -1;
    pipe.baseDistanceScale = [Infinity, -Infinity];
  });

  let queue = new PriorityQueue({
    comparator: (a, b) => a[0] - b[0]
  });

  let stack = [], cluster = 0;

  board.iterate(PlayerBase, base => {
    queue.queue([-base.vacuumForce, base.p, {
      x: base.p.x,
      y: base.p.y
    }]);
    stack.push([base.p, cluster++]);
  });

  while (queue.length) {
    let [aspiration, coord] = queue.dequeue();
    for (let dir of Direction.ALL) {
      let x = coord.x + dir.x, y = coord.y + dir.y;
      if (!board.isPlayable(x, y))
        continue;
      let cell = board.get(x, y);
      if (cell instanceof Pipe) {
        if (cell.baseDistance === Infinity) {
          cell.baseDistance = aspiration + 1;
          queue.queue([aspiration + 1, {x: x, y: y}]);
        }
      }
    }
  }

  let clusters = new Map();
  while (stack.length) {
    let [coord, cluster] = stack.pop();
    for (let dir of Direction.ALL) {
      let x = coord.x + dir.x, y = coord.y + dir.y, cell = board.get(x, y);
      if (cell instanceof Pipe) {
        if (cell.cluster === -1) {
          cell.cluster = cluster;
          if (clusters.has(cluster)) {
            clusters.get(cluster).push(cell);
          } else {
            clusters.set(cluster, [cell]);
          }
          stack.push([cell.p, cluster]);
        }
      }
    }
  }
  for (let cluster of clusters.values()) {
    let min = Infinity, max = -Infinity;
    for (let pipe of cluster) {
      min = Math.min(min, pipe.baseDistance);
      max = Math.max(max, pipe.baseDistance);
    }
    for (let pipe of cluster)
      pipe.baseDistanceScale = [min, max + 1];
  }

  // pre-compute pulsar loots
  board.iterate(Pulsar, pulsar => {
    pulsar.energyPaths = [];
    for (let dir of Direction.ALL) {
      let cell = board.get(pulsar.p.x + dir.x, pulsar.p.y + dir.y);
      if (!(cell instanceof Pipe || cell instanceof PlayerBase))
        continue;
      if (cell.baseDistance !== Infinity) {
        pulsar.energyPaths.push(cell.p);
      }
    }
  });

  // pre-compute pipe energy paths
  board.iterate(Pipe, rootPipe => {
    let paths = [];
    if (!rootPipe.vacuuming()) {
      rootPipe.energyPaths = paths;
      return;
    }
    let move = (pipe, factor, speed) => {
      if (speed === 0 || pipe instanceof PlayerBase)
        return;
      // split energy if needed (same algo as above)
      let directions = [], lowestDistance = Infinity;
      for (let dir of Direction.ALL) {
        let cell = board.get(pipe.p.x + dir.x, pipe.p.y + dir.y);
        if (!(cell instanceof Pipe || cell instanceof PlayerBase))
          continue;
        if (cell.vacuuming()) {
          if (cell.baseDistance < lowestDistance) {
            directions = [dir];
            lowestDistance = cell.baseDistance;
          } else if (cell.baseDistance === lowestDistance) {
            directions.push(dir);
          }
        }
      }
      if (directions.length) {
        for (let dir of directions) {
          let other = board.get(pipe.p.x + dir.x, pipe.p.y + dir.y);
          paths.push([pipe.p, other.p, factor / directions.length, speed]);
          move(other, factor / directions.length, speed - 1);
        }
      }
    };
    move(rootPipe, 1, rootPipe.speed);
    rootPipe.energyPaths = paths;
  });

  // update
  board.iterate(Entity, entity => entity.update());
}

class GameManager {
  constructor(n, maxTurn) {
    const baseN = parseInt(n / 3);
    const innerSize = n * gs, outerSize = innerSize + baseZoneMargin * 2;

    let that = this;

    this.built = [];
    this.destroyed = [];
    this.updated = [];
    this.movingForward = false;

    game = new Phaser.Game(outerSize, outerSize, Phaser.CANVAS, 'main', {
      preload: preload,
      create: create,
      update: update
    }, false, true);

    var energyGraphics;

    var ui = this.ui = {};
    this.maxTurn = maxTurn + 1; // FIXME: fuck logic
    this.tick = 0;

    function updateUiCellProps(cell) {
      function prop(name, value) {
        ui.cellProps.append($('<dt>').text(name));
        ui.cellProps.append($('<dd>').text(value));
      }

      function plural(q, p = 's', s = '') {
        return q <= 1 ? s : p;
      }

      ui.cellProps.empty();
      if (!cell || cell instanceof Void) {
        ui.cellInfo.css('visibility', 'hidden');
        return;
      }
      ui.cellInfo.css('visibility', 'visible');
      ui.cellImg.attr('class', '');
      if (cell instanceof Pipe) {
        if (cell instanceof SuperPipe) {
          ui.cellType.text("Super-tuyau");
          ui.cellImg.addClass('cell-super-pipe');
        } else {
          ui.cellType.text("Tuyau");
          ui.cellImg.addClass('cell-pipe');
        }
        prop("Débit", cell.speed);
        prop("Charge transportée", cell.energy);
      } else if (cell instanceof Debris) {
        ui.cellImg.addClass('cell-destroyed-pipe');
        ui.cellType.text("Débris de tuyau");
      } else if (cell instanceof Pulsar) {
        ui.cellType.text("Pulsar");
        ui.cellImg.addClass('cell-pulsar');
        prop("Période", "" + cell.period + " tours");
        let tl = cell.period - 1 - ((turn - 1) % cell.period);
        prop("Émet", (cell.stock === 0) ? "n/a" : (tl === 0) ? "maintenant" : "dans " + tl + " tour" + plural(tl));
        prop("Charge par pulse", cell.energy);
        prop("Capacité", (cell.stock > 0) ? cell.stock + " pulse" + plural(cell.stock) : "mort");
      } else if (cell instanceof PlayerBase) {
        ui.cellType.text("Base");
        ui.cellImg.addClass('cell-base');
        prop("Force d'aspiration", cell.vacuumForce);
      }
    }

    function preload() {
      // disable context menu
      game.renderer.view.oncontextmenu = (e) => e.preventDefault();
      let $controls = $('#sidebar');
      $controls.css('left', $('#main').width());

      ui.spConnecting = $('#sp-connecting');
      ui.spWaitingPlayer = $('#sp-waiting');
      ui.controls = $('#controls');
      ui.mouseCoords = $('#ui-mouse-coords');
      ui.cellInfo = $('#ui-cell-info');
      ui.cellImg = $('#ui-cell-img');
      ui.cellType = $('#ui-cell-type');
      ui.cellProps = $('#ui-cell-props');
      ui.progressBar = $('#ui-game-progress-bar');
      ui.timeline = $('#ui-game-timeline');
      ui.turn = $('#ui-turn');
      ui.turnMax = $('#ui-turn-max');
      ui.round = $('#ui-round');
      ui.roundMax = $('#ui-round-max');
      ui.players = [];
      ui.spGameOver = $('#sp-gameover');
      ui.goPlayers = [];

      ui.spWaitingPlayer.hide();
      ui.spGameOver.hide();

      for (let player of Players) {
        let $player = $('.ui-player.player-' + player.uiid);
        let color = '#' + player.color.toString(16);
        ui.players.push($player);
        $player.find('.ui-player-marker').css('backgroundColor', color);
        let $goPlayer = $('.go-player.player-' + player.uiid);
        ui.goPlayers.push($goPlayer);
        $goPlayer.css('color', color);
      }

      // load assets
      game.load.path = 'assets/';
      game.load.image('pixel.energy', 'pixel.energy.png');
      game.load.image('aura', 'aura.png');
      game.load.image('brick', 'brick.png');
      game.load.image('screen', 'screen.png');
      game.load.image('debris', 'debris.png');
      game.load.image('backhoe', 'backhoe.png');
      game.load.spritesheet('pipes', 'pipes.png', 32, 32);
      game.load.spritesheet('energy', 'energy.tileset.png', 32, 32);
      game.load.spritesheet('platform', 'platform.png', 32, 32);
      game.load.spritesheet('boom', 'boom.png', 32, 32);
    }

    function create() {
      bgLayer = game.add.group();
      playZoneLayer = game.add.group();
      baseBgLayer = game.add.group();
      pulsarLayer = game.add.group();
      pipeEnergyLayer = game.add.group();
      baseLayer = game.add.group();
      pipeLayer = game.add.group();
      effectLayer = game.add.group();
      debugLayer = game.add.group();
      mouseCellLayer = game.add.group();

      for (let group of [pulsarLayer, pipeEnergyLayer, baseBgLayer, baseLayer, pipeLayer, effectLayer, debugLayer, mouseCellLayer]) {
        group.x = baseZoneMargin;
        group.y = baseZoneMargin;
      }

      // star field
      let bg = game.make.bitmapData(outerSize, outerSize, 'starField', true);
      StarField(bg.ctx, outerSize, outerSize)(400);
      game.add.image(0, 0, bg, undefined, bgLayer);

      // grid
      let grid = game.make.bitmapData(innerSize, innerSize);
      grid.ctx.fillStyle = 'white';
      for (let i = 1; i < n - 1; i++)
        for (let j = 1 + i % 2; j < n - 1; j += 2) {
          grid.ctx.fillRect(i * gs, j * gs, gs, gs);
        }
      let img = grid.addToWorld(baseZoneMargin, baseZoneMargin);
      img.alpha = 0.05;

      energyGraphics = game.add.graphics(0, 0, pipeEnergyLayer);

      // mouse cell highlight
      mouseCell = game.add.graphics(0, 0, mouseCellLayer);
      mouseCell.beginFill(0xffffff, .6);
      mouseCell.blendMode = Phaser.blendModes.LIGHTEN;
      mouseCell.drawRect(0, 0, gs, gs);
      mouseCell.endFill();

      let baseCenter = Math.floor(n / 2 - baseN / 2);
      PlayerBase.build(0, Players[0].color, baseCenter, 0, baseN, Orientation.H, false);
      PlayerBase.build(0, Players[0].color, baseCenter, n - 1, baseN, Orientation.H, true);
      PlayerBase.build(1, Players[1].color, 0, baseCenter, baseN, Orientation.V, false);
      PlayerBase.build(1, Players[1].color, n - 1, baseCenter, baseN, Orientation.V, true);

      /* Keyboard press */
      game.input.keyboard.addKey(Phaser.Keyboard.M).onDown.add(() => {
        displayMode++;
        displayMode %= 2;
        board.iterate(Pipe, e => e.update());
      });
      game.input.keyboard.addKey(Phaser.Keyboard.C).onDown.add(() => {
        board.iterate(Pipe, pipe => {
          if (pipe.baseDistance === Infinity) {
            board.removeC(pipe.p);
          }
        });
        buildPipeGraph();
      });
      game.input.keyboard.addKey(Phaser.Keyboard.SPACEBAR).onDown.add(() => that.moveForward());
      game.input.keyboard.addKey(Phaser.Keyboard.ESC).onDown.add(() => {
        that.ui.spGameOver.fadeOut('fast');
      });

      game.input.mouse.mouseWheelCallback = (e) => {
        let pos = sceneCoordToCell(game.input), cell = board.getC(pos);
        let sign = (e.wheelDeltaY > 0 ? 1 : -1);
        if (cell instanceof PlayerBase) {
          cell.setVacuumForce(Math.max(0, cell.vacuumForce + sign));
          cell.update();
          buildPipeGraph();
        } else if (cell instanceof SuperPipe) {
          cell.speed = Math.max(0, cell.speed + sign);
        } else {
          ticksPerState = Phaser.Math.clamp(ticksPerState + sign, 1, 400);
        }
      };

      /* Mouse click */
      game.input.addMoveCallback(e => {
        if (game.input.mousePointer.isDown && game.input.mousePointer.button == 0) {
          // left click: build
          let down = (k) => game.input.keyboard.isDown(Phaser.Keyboard[k]);
          let entity = down('I') ? Pulsar : down('O') ? Pipe : down('P') ? SuperPipe : null;
          if (!entity)
            return;
          let pos = sceneCoordToCell(game.input);
          if (board.isPlayableC(pos)) {
            if (!(board.getC(pos) instanceof Void))
              board.removeC(pos);
            let cell = new entity(pos);
            board.set(cell);
            updateCellNeigh(pos);
            buildPipeGraph();
          }
        } else if (game.input.mousePointer.isDown && game.input.mousePointer.button == 2) {
          // right click: destroy
          let pos = sceneCoordToCell(game.input), cell = board.getC(pos);
          if (board.isPlayableC(pos)) {
            board.removeC(pos);
            if (cell instanceof Pipe) {
              let s = game.add.sprite(pos.x * (.5 + gs), pos.y * (.5 + gs), 'boom', 0, effectLayer);
              s.anchor.set(.5);
              let a = s.animations.add('explode');
              a.parent = s;
              a.killOnComplete = true;
              s.animations.play('explode', 15, false, true);
              let e = game.add.emitter(baseZoneMargin + pos.x * (.5 + gs), baseZoneMargin + pos.y * (.5 + gs), 10);
              e.setScale(.1, .5);
              e.setRotation(0, 360);
              e.setAlpha(1, 0, 1500);
              e.gravity = 0;
              e.makeParticles('brick');
              e.explode(800, 10);
              board.setC(new Debris(pos));
            } else if (cell instanceof Debris) {
              let s = game.add.sprite(pos.x * gs - gs / 2, pos.y * gs, 'backhoe', 0, effectLayer);
              s.scale.set(spriteScale);
              s.alpha = 0;
              let t = game.make.tween(s).to({x: '+' + gs}, ticksPerState * 8, Phaser.Easing.Linear.None, true);
              game.make.tween(s).to({alpha: 1}, ticksPerState, Phaser.Easing.Linear.None, true);
              t.onComplete.add(e => s.destroy());
            }
            updateCellNeigh(pos);
            buildPipeGraph();
          }
        }
      });

      // let's go
      network.hello();
    }

    var mouseCoords = {};

    function update() {
      let mouse = game.input;
      let mouseInPlayzone = mouse.x > baseZoneMargin + gs && mouse.x < baseZoneMargin + innerSize - gs &&
        mouse.y > baseZoneMargin + gs && mouse.y < baseZoneMargin + innerSize - gs;

      let pos = sceneCoordToCell(mouse), cell = board.getC(pos);
      if (pos.x !== mouseCoords.x || pos.y !== mouseCoords.y) {
        mouseCoords = Phaser.Utils.extend({}, pos);
        shouldUpdateUi = true;
      }

      if (shouldUpdateUi) {
        shouldUpdateUi = false;
        updateUiCellProps(cell);
        if (pos.x >= 0 && pos.x < n && pos.y >= 0 && pos.y < n)
          ui.mouseCoords.text(('00' + pos.x).slice(-2) + ',' + ('00' + pos.y).slice(-2));
        if (mouseInPlayzone) {
          mouseCell.x = gs * pos.x;
          mouseCell.y = gs * pos.y;
        }
      }

      if (!that.movingForward)
        return;

      let tf = that.tick / ticksPerState;

      // fade in player actions
      if (turnState === TurnState.ACTIONS_DONE) {
        that.built.map(cell => {
          cell.alpha = tf;
          cell.update();
        });
        that.destroyed.map(p => {
          var cell = board.getC(p);
          cell.alpha = 1 - tf;
          cell.update();
        });
      }

      // Plasma move
      if (true) {
        let tfPlasma = (turnState === TurnState.PLASMA_MOVED) ? tf : 0;
        let tfPulsar = (turnState === TurnState.PULSARS_LOOTED) ? tf : 1;
        energyGraphics.clear();
        energyGraphics.beginFill(0xffffff, 0.8);
        let draw = (p, scale) => {
          if (scale === 0)
            return;
          let r = gs / 2 * Math.max(0.2, Math.min(.8, scale * 0.5));
          energyGraphics.drawCircle((p.x + .5) * gs, (p.y + .5) * gs, r);
        };
        let segmentTween = (a, b, t) => {
          t = Math.min(1, Math.max(0, t));
          return {x: (1 - t) * a.x + t * b.x, y: (1 - t) * a.y + t * b.y};
        };

        if (turnState === TurnState.PULSARS_LOOTED) {
          board.iterate(Pulsar, pulsar => {
            if (pulsar.stock <= 0 || ((turn) % pulsar.period) !== 0)
              return;
            for (let pos of pulsar.energyPaths) {
              draw(segmentTween(pulsar.p, pos, tfPulsar), ((cell instanceof PlayerBase) ? (tfPulsar < .5 ? 1 : 1 - tfPulsar + .5) : 1) * pulsar.energy);
            }
          });
        }

        board.iterate(Pipe, rootPipe => {
          for (let [from, to, factor, speed] of rootPipe.energyPaths) {
            const offset = rootPipe.speed - speed;
            if (offset / rootPipe.speed <= tfPlasma && tfPlasma <= (offset + 1) / rootPipe.speed)
              draw(segmentTween(from, to, (tfPlasma - offset / rootPipe.speed) * rootPipe.speed), rootPipe.energy * factor);
          }
        });
        energyGraphics.endFill();
      }

      that.tick++;
      if (that.tick === ticksPerState + 1) {
        console.log('ended state', turnState, 'for turn', turn);
        that.tick = 0;
        that.movingForward = false;

        if (turnState === TurnState.ACTIONS_DONE) {
          that.destroyed.map(p => {
            if (board.getC(p) instanceof Debris) {
              // backhoe animation
              let s = game.add.sprite(p.x * gs - gs / 2, p.y * gs, 'backhoe', 0, effectLayer);
              s.scale.set(spriteScale);
              s.alpha = 0;
              let t = game.make.tween(s).to({x: '+' + gs}, ticksPerState * 8, Phaser.Easing.Linear.None, true);
              game.make.tween(s).to({alpha: 1}, ticksPerState, Phaser.Easing.Linear.None, true);
              t.onComplete.add(e => s.destroy());
            }
            board.removeC(p);
          });

          that.built.map(entity => {
            if (entity instanceof Debris) {
              // explode animation
              let pos = entity.p;
              let s = game.add.sprite(pos.x * (.5 + gs), pos.y * (.5 + gs), 'boom', 0, effectLayer);
              s.anchor.set(.5);
              let a = s.animations.add('explode');
              a.parent = s;
              a.killOnComplete = true;
              let e = game.add.emitter(baseZoneMargin + pos.x * (.5 + gs), baseZoneMargin + pos.y * (.5 + gs), 10);
              e.setScale(.1, .5);
              e.setRotation(0, 360);
              e.setAlpha(1, 0, 1500);
              e.gravity = 0;
              e.makeParticles('brick');
              e.explode(800, 10);
              a.onComplete.add(() => { console.log('destroy emitter', e); e.destroy(); });
              s.animations.play('explode', 15, false, true);
            }
            board.set(entity);
          });

          buildPipeGraph();

          board.iterate(Entity, entity => {
            entity.update();
          });
          shouldUpdateUi = true;

          if (autoForward || game.input.keyboard.isDown(Phaser.Keyboard.SPACEBAR))
            gameManager.moveForward();

        } else if (turnState === TurnState.PLASMA_MOVED) {

          that.updated.map(tuple => {
            let [p, modifier] = tuple;
            let cell = board.getC(p);
            modifier.call(undefined, cell);
          });

          shouldUpdateUi = true;

          if (autoForward || game.input.keyboard.isDown(Phaser.Keyboard.SPACEBAR))
            gameManager.moveForward();

        } else if (turnState === TurnState.PULSARS_LOOTED || turnState === TurnState.TURN_BEGINS) {

          if (autoForward || game.input.keyboard.isDown(Phaser.Keyboard.SPACEBAR))
            gameManager.moveForward();

        }
      }
    }
  }

  moveForward() {
    if (this.movingForward || turn >= this.maxTurn)
      return;

    if (turnState === TurnState.TURN_BEGINS) {

      // apply player actions
      var that = this;
      turnState = TurnState.ACTIONS_DONE;
      this.ui.timeline.attr('class', 'ui-tl-1');

      this.built = [];
      this.destroyed = [];
      this.updated = [];

      nextMap.map((data, i) => {
        let y = i % board.n, x = Math.floor(i / board.n), type = parseInt(data.type);
        let cell = board.get(x, y), p = new Phaser.Point(x, y);
        switch (type) {
          case CellType.VOID:
            if (!(cell instanceof Void)) {
              that.destroyed.push(p);
            }
            break;
          case CellType.PIPE:
            if (cell instanceof SuperPipe || !(cell instanceof Pipe)) {
              that.destroyed.push(p);
              let pipe = new Pipe(p);
              pipe.alpha = 0;
              that.built.push(pipe);
              if (data.plasma > 0) {
                that.updated.push([p, pipe => {
                  pipe.energy = data.plasma
                }]);
              }
            } else if (cell.energy !== data.plasma) {
              that.updated.push([p, pipe => {
                pipe.energy = data.plasma
              }]);
            }
            break;
          case CellType.SUPER_PIPE:
            if (!(cell instanceof SuperPipe)) {
              let pipe = new SuperPipe(p);
              pipe.alpha = 0;
              that.built.push(pipe);
              if (data.plasma > 0) {
                that.updated.push([p, pipe => {
                  pipe.energy = data.plasma
                }]);
              }
            } else if (cell.energy !== data.plasma) {
              that.updated.push([p, pipe => {
                pipe.energy = data.plasma
              }]);
            }
            break;
          case CellType.PULSAR:
            console.assert(cell instanceof Pulsar);
            break;
          case CellType.DEBRIS:
            if (!(cell instanceof Debris)) {
              that.destroyed.push(p);
              let d = new Debris(p);
              d.alpha = 0;
              that.built.push(d);
            }
            break;
          case CellType.BASE:
            console.assert(cell instanceof PlayerBase, x, y, cell);
            if (data.vacuum !== cell.vacuumForce)
              that.updated.push([p, base => { base.vacuumForce = data.vacuum}]);
            break;
          case CellType.NOPE:
            break;
          default:
            console.warn("Unknown cell type", type);
        }
      });

    } else if (turnState === TurnState.ACTIONS_DONE) {

      // move plasma
      turnState = TurnState.PLASMA_MOVED;
      this.ui.timeline.attr('class', 'ui-tl-2');

    } else if (turnState === TurnState.PLASMA_MOVED) {

      this.ui.timeline.attr('class', 'ui-tl-3');
      // pulsate pulsars
      turnState = TurnState.PULSARS_LOOTED;

    } else if (turnState === TurnState.PULSARS_LOOTED) {

      // next turn
      this.ui.timeline.attr('class', '');
      turnState = TurnState.TURN_BEGINS;
      players = nextPlayers;
      this.updateUi();
      // fixme: should use another method name
      network.hello();

    }
    this.movingForward = true;
  }

  connected() {
    this.ui.spConnecting.fadeOut(() => $('#main,#sidebar').fadeIn());
  }

  showWaiting(show) {
    if (show) {
      this.ui.spWaitingPlayer.fadeIn('fast');
    } else {
      this.ui.spWaitingPlayer.fadeOut('fast');
    }
  }

  updateUi() {
    this.ui.turn.text(turn);
    this.ui.turnMax.text(this.maxTurn);
    this.ui.round.text(1 + Math.floor((turn - 1) / 2));
    this.ui.roundMax.text(Math.floor(this.maxTurn / 2));
    this.ui.progressBar.css('width', (turn / this.maxTurn * 100) + '%');

    if (turn >= this.maxTurn) {
      this.ui.spGameOver.fadeIn('slow');
    }

    let player = this.ui.players[(turn) % 2], other = this.ui.players[(turn + 1) % 2];
    player.addClass('active');
    other.removeClass('active');
    for (let i = 0; i < players.length; i++) {
      let $player = this.ui.players[i], $goPlayer = this.ui.goPlayers[i];
      let name = players[i].name, score = players[i].score;
      if (score > players[(i + 1) % 2].score) {
        $goPlayer.addClass('winner');
      }
      $player.find('.ui-player-name').text(name);
      $goPlayer.find('.go-player-name').text(name);
      $player.find('.ui-player-score').text(score);
      $goPlayer.find('.go-player-score').text(score);
    }
  }
}

// let's do this™
network = new Network('ws://127.0.0.1:8099/ws');
