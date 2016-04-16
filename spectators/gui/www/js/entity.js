/**
 * Copyright Prologin 2016 - Alexandre Macabies
 *
 * Game entities.
 */

class Entity {
  constructor(p) {
    this.p = p;
    this.e = [];
    this.alpha = 1;
  }

  remove() {
    this.e.map(e => e.destroy());
    delete this;
  }

  _update() {
  }

  forward() {
  }

  update() {
    this._update({
      N: board.get(this.p.x, this.p.y - 1),
      E: board.get(this.p.x + 1, this.p.y),
      S: board.get(this.p.x, this.p.y + 1),
      W: board.get(this.p.x - 1, this.p.y)
    });
  }
}

class Void extends Entity {
  constructor(p) {
    super(p);
  }
}

class Pulsar extends Entity {
  constructor(p) {
    super(p);
    this.period = 3;
    this.energy = 1;
    this.stock = Infinity;
    this.energyPaths = [];

    let s = this.sprite = game.add.sprite(p.x * gs, p.y * gs, 'energy', 0, pulsarLayer);
    s.scale.set(spriteScale);
    s.animations.add('pulsate');
    s.animations.play('pulsate', 30, true);
    this.e.push(s);

    this.emitters = {};
    for (let dn of Direction.NAMES) {
      let dir = Direction[dn];
      let em = this.emitters[dn] = game.add.emitter(gs * (.5 + p.x) + dir.x * 4 + baseZoneMargin, gs * (.5 + p.y) + dir.y * 4 + baseZoneMargin, 50);
      em.on = false;
      em.gravity = 0;
      em.setRotation(0, 0);
      em.setAlpha(.3, 1);
      em.setScale(.1, 1.2);
      em.width = 4;
      em.height = 4;
      em.setXSpeed(dir.x * 40, dir.x * 50);
      em.setYSpeed(dir.y * 40, dir.y * 50);
      em.makeParticles('pixel.energy', 0, 10);
      em.lifespan = 200;
      this.e.push(em);
    }
    this.update();
  }

  _update(n) {
    for (let dn of Direction.NAMES) {
      this.emitters[dn].on = this.stock > 0 && (n[dn] instanceof Pipe || n[dn] instanceof PlayerBase) && n[dn].vacuuming();
    }
  }

  update() {
    super.update();
    this.sprite.alpha = this.stock > 0 ? this.alpha : 0.4;
    if (this.stock === 0)
      this.sprite.animations.stop();
  }
}

class Pipe extends Entity {
  constructor(p) {
    super(p);
    this.baseDistance = Infinity;
    this.baseDistanceScale = [0, 1];
    this.speed = 1;
    this.energy = 0;
    this.frameOffset = 0;
    this.energyPaths = [];
    let s = this.sprite = game.add.sprite(p.x * gs, p.y * gs, 'pipes', 0, pipeLayer);
    s.scale.set(spriteScale);
    this.e.push(s);

    let energy = this.energyGroup = game.add.group(pipeEnergyLayer);
    this.e.push(energy);
    energy.x = p.x * gs;
    energy.y = p.y * gs;
    this.energyText = game.add.text(0, 0, '' + this.energy, {
      fill: 'black',
      boundsAlignH: 'center',
      boundsAlignV: 'middle',
      font: 'monospace'
    }, energy);
    this.energyText.setTextBounds(0, 0, gs, gs);
    this.energyText.visible = false;
    this.update();
  }

  vacuuming() {
    return this.baseDistance !== Infinity;
  }

  update() {
    super.update();
    this.sprite.alpha = this.alpha;
    this.energyGroup.alpha = this.alpha;

    if (displayMode === DisplayMode.HEATMAP) {
      let [min, max] = this.baseDistanceScale;
      let hue = this.baseDistance === Infinity ? 0 : .9 - .9 * (this.baseDistance - min) / (max - min);
      let c = Phaser.Color.HSLtoRGB(hue, 1, .5);
      this.sprite.tint = (c.r << 16) + (c.g << 8) + c.b;
    } else {
      this.sprite.tint = 0xffffff;
    }
  }

  _update(n) {
    // set the right frame
    let frame = (displayMode === DisplayMode.HEATMAP ? 0 : this.frameOffset);
    Direction.NAMES.map((dir, i) => {
      let cell = n[dir];
      if (cell instanceof Pipe || cell instanceof PlayerBase || (this.vacuuming() && cell instanceof Pulsar))
        frame += 1 << i;
    });
    this.sprite.frame = frame;
  }

  destroy() {
    super.destroy();
  }
}

class SuperPipe extends Pipe {
  constructor(p) {
    super(p);
    this.speed = 2;
    this.frameOffset = 20;
  }
}

class PlayerBase extends Entity {
  constructor(p, player, orientation, flip) {
    super(p);
    this.player = player;
    this.setVacuumForce(1);

    this.direction = orientation === Orientation.H ? Direction.S : Direction.E;
    this.spriteFrame = 16 + (orientation === Orientation.H ? (flip ? 0 : 2) : (flip ? 3 : 1));
    let s = this.sprite = game.add.sprite(p.x * gs, p.y * gs, 'pipes', 0, baseLayer);
    this.e.push(s);
    s.scale.set(spriteScale);
    if (flip) {
      this.direction = Direction.flip(this.direction);
    }

    let vacGaugeGroup = game.add.group(baseLayer);
    this.e.push(vacGaugeGroup);
    vacGaugeGroup.x = (p.x - this.direction.x) * gs;
    vacGaugeGroup.y = (p.y - this.direction.y) * gs;
    let gaugeSprite = game.add.sprite(0, 0, 'screen', 0, vacGaugeGroup);
    gaugeSprite.scale.set(spriteScale);
    let vacGauge = this.vacGauge = game.add.text(0, 0, '0', {
      font: 'monospace',
      fontSize: 10,
      fill: 'white',
      boundsAlignH: 'center',
      boundsAlignV: 'middle'
    }, vacGaugeGroup);
    vacGauge.setTextBounds(0, 2, gs, gs);
    this.update();
  }

  vacuuming() {
    return true;
  }

  setVacuumForce(force) {
    this.vacuumForce = force;
    this.baseDistance = -this.vacuumForce;
  }

  update() {
    this.vacGauge.text = '' + this.vacuumForce;
    let dir = this.direction, cell = board.get(this.p.x + dir.x, this.p.y + dir.y);
    if (cell instanceof Pipe || cell instanceof Pulsar) {
      this.sprite.frame = this.spriteFrame;
    } else {
      this.sprite.frame = 0;
    }
  }

  static build(player, color, x, y, size, orientation, flip) {
    if (size < 2)
      throw "base size too small";

    let dir = orientation === Orientation.H ? {x: 1, y: 0} : {x: 0, y: 1};

    let bg = (x, y, frame) => {
      let s = game.add.sprite(x * gs, y * gs, 'platform', frame, baseBgLayer);
      s.scale.set(spriteScale);
    };

    let build = (x, y) => {
      let pb = new PlayerBase(new Phaser.Point(x, y), player, orientation, flip);
      board.set(pb);
    };

    bg(x - dir.x, y - dir.y, orientation === Orientation.H ? (flip ? 0 : 6) : (flip ? 0 : 2));
    bg(x + dir.x * size, y + dir.y * size, orientation === Orientation.H ? (flip ? 2 : 8) : (flip ? 6 : 8));

    bg(x - dir.x + (flip ? 1 : -1) * dir.y, y - dir.y + (flip ? 1 : -1) * dir.x, orientation === Orientation.H ? 3 : 1);
    bg(x - dir.x + (flip ? 2 : -2) * dir.y, y - dir.y + (flip ? 2 : -2) * dir.x, orientation === Orientation.H ? 3 : 1);

    bg(x + dir.x * size + (flip ? 1 : -1) * dir.y, y + dir.y * size + (flip ? 1 : -1) * dir.x, orientation === Orientation.H ? 5 : 7);
    bg(x + dir.x * size + (flip ? 2 : -2) * dir.y, y + dir.y * size + (flip ? 2 : -2) * dir.x, orientation === Orientation.H ? 5 : 7);

    let aura = game.add.sprite((x + dir.y * (flip ? 3 : 1)) * gs, (y + dir.x * (flip ? 3 : 1)) * gs, 'aura', baseBgLayer);
    if (orientation === Orientation.H) {
      aura.angle = 90;
    } else {
      aura.angle = 180;
    }
    aura.scale.set(spriteScale);
    aura.anchor.set(.5, .5);
    aura.tint = color;

    aura = game.add.sprite((x + (3 + size) * dir.x + dir.y * (flip ? 3 : 1)) * gs, (y + (3 + size) * dir.y + dir.x * (flip ? 3 : 1)) * gs, 'aura', baseBgLayer);
    if (orientation === Orientation.H) {
      aura.angle = -90;
    } else {
      aura.angle = 0;
    }
    aura.scale.set(spriteScale);
    aura.anchor.set(.5, .5);
    aura.tint = color;

    for (let i = 0; i < size; i++) {
      bg(x + dir.x * i, y + dir.y * i, orientation === Orientation.H ? (flip ? 1 : 7) : (flip ? 3 : 5));
      bg(x + dir.x * i + (flip ? 1 : -1) * dir.y, y + dir.y * i + (flip ? 1 : -1) * dir.x, 4);
      bg(x + dir.x * i + (flip ? 2 : -2) * dir.y, y + dir.y * i + (flip ? 2 : -2) * dir.x, 4);
      build(x + dir.x * i, y + dir.y * i);
    }
  }
}

class Debris extends Entity {
  constructor(p) {
    super(p);
    let s = this.sprite = game.add.sprite(p.x * gs, p.y * gs, 'debris', 0, pipeLayer);
    s.scale.set(spriteScale);
    this.e.push(s);
  }

  update() {
    super.update();
    this.sprite.alpha = this.alpha;
  }
}
