/**
 * Copyright Prologin 2016 - Alexandre Macabies
 *
 * Main player.
 */

Phaser.Game.prototype.showDebugHeader = () => null;

/* Game config */
const n = 39, baseN = 13;

/* Player UI config */
const gs = 20, baseZoneMargin = 1.5 * gs, spriteScale = gs / 32;
var ticksPerRound = 45;

const innerSize = n * gs, outerSize = innerSize + baseZoneMargin * 2;

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
const Player = {A: 0, B: 1};

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
        queue.queue([-base.vacuumForce, base.p, {x: base.p.x, y: base.p.y}]);
        stack.push([base.p, cluster++]);
    });

    while (queue.length) {
        let [aspiration, coord] = queue.dequeue();
        for (let dir of Direction.ALL) {
            let x = coord.x + dir.x, y = coord.y + dir.y;
            if (!Board.isPlayable(x, y))
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

    board.iterate(Entity, entity => entity.update());
}

class Board {
    constructor() {
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

    setC(c, cell) {
        this.set(c.x, c.y, cell);
    }

    set(x, y, cell) {
        console.assert(cell instanceof Entity);
        this.entities[[x, y]] = cell;
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

    static isPlayableC(p) {
        return Board.isPlayable(p.x, p.y);
    }

    static isPlayable(x, y) {
        return x >= 1 && x <= n - 2 && y >= 1 && y <= n - 2;
    }
}

var game = new Phaser.Game(outerSize, outerSize, Phaser.CANVAS, 'main', {
    preload: preload,
    create: create,
    update: update
}, false, true);

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

var energyGraphics;

var gameState = {
    turns: [],
    maxTurn: 100,
    players: {
        0: {name: 'Péon de la Rivière', color: 0xff71b0, score: 0},
        1: {name: 'Jean-Michel', color: 0x8aff73, score: 0},
    }
};

var ui = {};
var board = new Board(n);
var displayMode = DisplayMode.NORMAL;
var autoForward = false;
var forwarding = false;
var debug = {building: null};
var tick = 0, turn = 1;

function updateUi() {
    ui.turn.text(turn);
    ui.turnMax.text(gameState.maxTurn);
    ui.round.text(1 + Math.floor((turn - 1) / 2));
    ui.roundMax.text(Math.floor(gameState.maxTurn / 2));
    ui.progressBar.css('width', (turn / gameState.maxTurn * 100) + '%');

    let player = ui.players[turn % 2 == 0 ? Player.A : Player.B],
        other = ui.players[turn % 2 == 1 ? Player.A : Player.B];
    player.addClass('active');
    other.removeClass('active');
    // TODO: score from state
    // $player.find('.ui-player-score').text(data.score);
}

function updateUiCellProps(cell) {
    function prop(name, value) {
        ui.cellProps.append($('<dt>').text(name));
        ui.cellProps.append($('<dd>').text(value));
    }

    function plural(q, p='s', s='') {
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
    } else if (cell instanceof DestroyedPipe) {
        ui.cellImg.addClass('cell-destroyed-pipe');
        ui.cellType.text("Débris de tuyau");
    } else if (cell instanceof Pulsar) {
        ui.cellType.text("Pulsar");
        ui.cellImg.addClass('cell-pulsar');
        prop("Période", "" + cell.period + " tours");
        let tl = cell.turnsLeft();
        prop("Émet", (tl === 0) ? "maintenant" : "dans " + tl + " tour" + plural(tl));
        prop("Charge par pulse", cell.energy);
        prop("Capacité", cell.stock + " pulse" + plural(cell.stock));
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

    ui.mouseCoords = $('#ui-mouse-coords');
    ui.cellInfo = $('#ui-cell-info');
    ui.cellImg = $('#ui-cell-img');
    ui.cellType = $('#ui-cell-type');
    ui.cellProps = $('#ui-cell-props');
    ui.progressBar = $('#ui-game-progress-bar');
    ui.turn = $('#ui-turn');
    ui.turnMax = $('#ui-turn-max');
    ui.round = $('#ui-round');
    ui.roundMax = $('#ui-round-max');
    ui.players = {};

    for (let name in Player) {
        if (Player.hasOwnProperty(name)) {
            console.log(name);
            let $player = $('.player-' + name.toLowerCase()), data = gameState.players[Player[name]];
            ui.players[Player[name]] = $player;
            $player.find('.ui-player-marker').css('backgroundColor', '#' + data.color.toString(16));
            $player.find('.ui-player-name').text(data.name);
            $player.find('.ui-player-score').text(data.score);
        }
    }

    updateUi();

    // load assets
    game.load.path = 'assets/';
    game.load.image('pixel.energy', 'pixel.energy.png');
    game.load.image('aura', 'aura.png');
    game.load.image('brick', 'brick.png');
    game.load.image('screen', 'screen.png');
    game.load.image('ashes', 'ashes.png');
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
    PlayerBase.build(Player.A, gameState.players[Player.A].color, baseCenter, 0, baseN, Orientation.H, false);
    PlayerBase.build(Player.A, gameState.players[Player.A].color, baseCenter, n - 1, baseN, Orientation.H, true);
    PlayerBase.build(Player.B, gameState.players[Player.B].color, 0, baseCenter, baseN, Orientation.V, false);
    PlayerBase.build(Player.B, gameState.players[Player.B].color, n - 1, baseCenter, baseN, Orientation.V, true);

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
    game.input.keyboard.addKey(Phaser.Keyboard.SPACEBAR).onDown.add(() => {
        forwarding = true;
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
            ticksPerRound = Phaser.Math.clamp(ticksPerRound + sign, 1, 400);
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
            if (Board.isPlayableC(pos)) {
                if (!(board.getC(pos) instanceof Void))
                    board.removeC(pos);
                let cell = new entity(pos);
                board.setC(pos, cell);
                updateCellNeigh(pos);
                buildPipeGraph();
            }
        } else if (game.input.mousePointer.isDown && game.input.mousePointer.button == 2) {
            // right click: destroy
            let pos = sceneCoordToCell(game.input), cell = board.getC(pos);
            if (Board.isPlayableC(pos)) {
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
                    board.setC(pos, new DestroyedPipe(pos));
                } else if (cell instanceof DestroyedPipe) {
                    let s = game.add.sprite(pos.x * gs - gs / 2, pos.y * gs, 'backhoe', 0, effectLayer);
                    s.scale.set(spriteScale);
                    s.alpha = 0;
                    let t = game.make.tween(s).to({x: '+' + gs}, ticksPerRound * 8, Phaser.Easing.Linear.None, true);
                    game.make.tween(s).to({alpha: 1}, ticksPerRound, Phaser.Easing.Linear.None, true);
                    t.onComplete.add(e => s.destroy());
                }
                updateCellNeigh(pos);
                buildPipeGraph();
            }
        }
    });
}

var mouseCoords = {};
var shouldUpdateUi = false;

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
        updateUiCellProps(cell);
        if (mouseInPlayzone) {
            mouseCell.x = gs * pos.x;
            mouseCell.y = gs * pos.y;
            ui.mouseCoords.text(('00' + pos.x).slice(-2) + ',' + ('00' + pos.y).slice(-2));
        }
    }

    if (!forwarding)
        return;

    // Game forwarding
    // TODO: read from state instead!
    if (tick === 0) {
        board.iterate(Pipe, e => e.forwardEnergy());
        board.iterate(Pulsar, e => e.forward());
        board.iterate(Pipe, e => e.forward());
        turn++;
        updateUi();
        shouldUpdateUi = true;
    }

    // Plasma move
    if (true) {
        let tf = (tick % ticksPerRound) / ticksPerRound; // tween factor
        energyGraphics.clear();
        energyGraphics.beginFill(0xffffff, 0.8);
        let draw = (p, scale) => {
            if (scale === 0)
                return;
            let r = gs / 2 * Math.max(0.2, Math.min(.8, scale * 0.5)), np = Math.floor(Math.random() * 10) + 5, delta = Math.PI * 2 / np;
            energyGraphics.drawCircle((p.x + .5) * gs, (p.y + .5) * gs, r);
        };
        let segmentTween = (a, b, t) => {
            t = Math.min(1, Math.max(0, t));
            return {x: (1 - t) * a.x + t * b.x, y: (1 - t) * a.y + t * b.y};
        };

        board.iterate(Pulsar, pulsar => {
            if (!(pulsar.c === pulsar.period - 2))
                return;
            let directions = [];
            for (let dir of Direction.ALL) {
                let cell = board.get(pulsar.p.x + dir.x, pulsar.p.y + dir.y);
                if (!(cell instanceof Pipe || cell instanceof PlayerBase))
                    continue;
                if (cell.baseDistance !== Infinity) {
                    directions.push(dir);
                }
            }
            let energy = pulsar.energy;
            for (let dir of directions) {
                let cell = board.get(pulsar.p.x + dir.x, pulsar.p.y + dir.y);
                draw(segmentTween(pulsar.p, cell.p, tf), ((cell instanceof PlayerBase) ? (tf < .5 ? 1 : 1 - tf + .5) : 1) * energy);
            }
        });

        board.iterate(Pipe, rootPipe => {
            if (!rootPipe.vacuuming())
                return;
            let move = (pipe, energy, speed) => {
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
                    const offset = rootPipe.speed - speed;
                    for (let dir of directions) {
                        let other = board.get(pipe.p.x + dir.x, pipe.p.y + dir.y);
                        // only draw if it's in the right time slot
                        if (offset / rootPipe.speed <= tf && tf <= (offset + 1) / rootPipe.speed)
                            draw(segmentTween(pipe.p, other.p, (tf - offset / rootPipe.speed) * rootPipe.speed),
                                energy / directions.length);
                        // recursion, mother fucker, do you speak it?
                        move(other, energy / directions.length, speed - 1);
                    }
                }
            };
            move(rootPipe, rootPipe.energy, rootPipe.speed);
        });
        energyGraphics.endFill();
    }

    tick++;
    tick %= ticksPerRound;
    if (tick === ticksPerRound - 1) {
        forwarding = autoForward || game.input.keyboard.isDown(Phaser.Keyboard.SPACEBAR);
    }
}
