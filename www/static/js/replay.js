!function(t){if("object"==typeof exports&&"undefined"!=typeof module)module.exports=t();else if("function"==typeof define&&define.amd)define([],t);else{var e;e="undefined"!=typeof window?window:"undefined"!=typeof global?global:"undefined"!=typeof self?self:this,e.PriorityQueue=t()}}(function(){return function t(e,i,r){function o(n,s){if(!i[n]){if(!e[n]){var h="function"==typeof require&&require;if(!s&&h)return h(n,!0);if(a)return a(n,!0);var u=new Error("Cannot find module '"+n+"'");throw u.code="MODULE_NOT_FOUND",u}var p=i[n]={exports:{}};e[n][0].call(p.exports,function(t){var i=e[n][1][t];return o(i?i:t)},p,p.exports,t,e,i,r)}return i[n].exports}for(var a="function"==typeof require&&require,n=0;n<r.length;n++)o(r[n]);return o}({1:[function(t,e,i){var r,o,a,n,s,h=function(t,e){function i(){this.constructor=t}for(var r in e)u.call(e,r)&&(t[r]=e[r]);return i.prototype=e.prototype,t.prototype=new i,t.__super__=e.prototype,t},u={}.hasOwnProperty;r=t("./PriorityQueue/AbstractPriorityQueue"),o=t("./PriorityQueue/ArrayStrategy"),n=t("./PriorityQueue/BinaryHeapStrategy"),a=t("./PriorityQueue/BHeapStrategy"),s=function(t){function e(t){t||(t={}),t.strategy||(t.strategy=n),t.comparator||(t.comparator=function(t,e){return(t||0)-(e||0)}),e.__super__.constructor.call(this,t)}return h(e,t),e}(r),s.ArrayStrategy=o,s.BinaryHeapStrategy=n,s.BHeapStrategy=a,e.exports=s},{"./PriorityQueue/AbstractPriorityQueue":2,"./PriorityQueue/ArrayStrategy":3,"./PriorityQueue/BHeapStrategy":4,"./PriorityQueue/BinaryHeapStrategy":5}],2:[function(t,e,i){var r;e.exports=r=function(){function t(t){var e;if(null==(null!=t?t.strategy:void 0))throw"Must pass options.strategy, a strategy";if(null==(null!=t?t.comparator:void 0))throw"Must pass options.comparator, a comparator";this.priv=new t.strategy(t),this.length=(null!=t&&null!=(e=t.initialValues)?e.length:void 0)||0}return t.prototype.queue=function(t){this.length++,this.priv.queue(t)},t.prototype.dequeue=function(t){if(!this.length)throw"Empty queue";return this.length--,this.priv.dequeue()},t.prototype.peek=function(t){if(!this.length)throw"Empty queue";return this.priv.peek()},t.prototype.clear=function(){return this.length=0,this.priv.clear()},t}()},{}],3:[function(t,e,i){var r,o;o=function(t,e,i){var r,o,a;for(o=0,r=t.length;r>o;)a=o+r>>>1,i(t[a],e)>=0?o=a+1:r=a;return o},e.exports=r=function(){function t(t){var e;this.options=t,this.comparator=this.options.comparator,this.data=(null!=(e=this.options.initialValues)?e.slice(0):void 0)||[],this.data.sort(this.comparator).reverse()}return t.prototype.queue=function(t){var e;e=o(this.data,t,this.comparator),this.data.splice(e,0,t)},t.prototype.dequeue=function(){return this.data.pop()},t.prototype.peek=function(){return this.data[this.data.length-1]},t.prototype.clear=function(){this.data.length=0},t}()},{}],4:[function(t,e,i){var r;e.exports=r=function(){function t(t){var e,i,r,o,a,n,s,h,u;for(this.comparator=(null!=t?t.comparator:void 0)||function(t,e){return t-e},this.pageSize=(null!=t?t.pageSize:void 0)||512,this.length=0,h=0;1<<h<this.pageSize;)h+=1;if(1<<h!==this.pageSize)throw"pageSize must be a power of two";for(this._shift=h,this._emptyMemoryPageTemplate=e=[],i=r=0,n=this.pageSize;n>=0?n>r:r>n;i=n>=0?++r:--r)e.push(null);if(this._memory=[],this._mask=this.pageSize-1,t.initialValues)for(s=t.initialValues,o=0,a=s.length;a>o;o++)u=s[o],this.queue(u)}return t.prototype.queue=function(t){this.length+=1,this._write(this.length,t),this._bubbleUp(this.length,t)},t.prototype.dequeue=function(){var t,e;return t=this._read(1),e=this._read(this.length),this.length-=1,this.length>0&&(this._write(1,e),this._bubbleDown(1,e)),t},t.prototype.peek=function(){return this._read(1)},t.prototype.clear=function(){this.length=0,this._memory.length=0},t.prototype._write=function(t,e){var i;for(i=t>>this._shift;i>=this._memory.length;)this._memory.push(this._emptyMemoryPageTemplate.slice(0));return this._memory[i][t&this._mask]=e},t.prototype._read=function(t){return this._memory[t>>this._shift][t&this._mask]},t.prototype._bubbleUp=function(t,e){var i,r,o,a;for(i=this.comparator;t>1&&(r=t&this._mask,t<this.pageSize||r>3?o=t&~this._mask|r>>1:2>r?(o=t-this.pageSize>>this._shift,o+=o&~(this._mask>>1),o|=this.pageSize>>1):o=t-2,a=this._read(o),!(i(a,e)<0));)this._write(o,e),this._write(t,a),t=o},t.prototype._bubbleDown=function(t,e){var i,r,o,a,n;for(n=this.comparator;t<this.length;)if(t>this._mask&&!(t&this._mask-1)?i=r=t+2:t&this.pageSize>>1?(i=(t&~this._mask)>>1,i|=t&this._mask>>1,i=i+1<<this._shift,r=i+1):(i=t+(t&this._mask),r=i+1),i!==r&&r<=this.length)if(o=this._read(i),a=this._read(r),n(o,e)<0&&n(o,a)<=0)this._write(i,e),this._write(t,o),t=i;else{if(!(n(a,e)<0))break;this._write(r,e),this._write(t,a),t=r}else{if(!(i<=this.length))break;if(o=this._read(i),!(n(o,e)<0))break;this._write(i,e),this._write(t,o),t=i}},t}()},{}],5:[function(t,e,i){var r;e.exports=r=function(){function t(t){var e;this.comparator=(null!=t?t.comparator:void 0)||function(t,e){return t-e},this.length=0,this.data=(null!=(e=t.initialValues)?e.slice(0):void 0)||[],this._heapify()}return t.prototype._heapify=function(){var t,e,i;if(this.data.length>0)for(t=e=1,i=this.data.length;i>=1?i>e:e>i;t=i>=1?++e:--e)this._bubbleUp(t)},t.prototype.queue=function(t){this.data.push(t),this._bubbleUp(this.data.length-1)},t.prototype.dequeue=function(){var t,e;return e=this.data[0],t=this.data.pop(),this.data.length>0&&(this.data[0]=t,this._bubbleDown(0)),e},t.prototype.peek=function(){return this.data[0]},t.prototype.clear=function(){this.length=0,this.data.length=0},t.prototype._bubbleUp=function(t){for(var e,i;t>0&&(e=t-1>>>1,this.comparator(this.data[t],this.data[e])<0);)i=this.data[e],this.data[e]=this.data[t],this.data[t]=i,t=e},t.prototype._bubbleDown=function(t){var e,i,r,o,a;for(e=this.data.length-1;;){if(i=(t<<1)+1,o=i+1,r=t,e>=i&&this.comparator(this.data[i],this.data[r])<0&&(r=i),e>=o&&this.comparator(this.data[o],this.data[r])<0&&(r=o),r===t)break;a=this.data[r],this.data[r]=this.data[t],this.data[t]=a,t=r}},t}()},{}]},{},[1])(1)});

$(function () {
  const N_CELLS = 39;
  const CELL_SIZE = 18;
  const N_BASES = parseInt(N_CELLS / 3);
  const TURN_DURATION = 250;
  const B_WIDTH = 2, B_PAD = .5, B_OFFSET = .3;
  const PLAYER_COLORS = ['#ff71b0', '#8aff73'];

  const DIRECTIONS = [{x: 0, y: -1}, {x: 1, y: 0}, {x: 0, y: 1}, {x: -1, y: 0}];
  const CellType = {
    VOID: 0,
    PIPE: 1,
    SUPER_PIPE: 2,
    DEBRIS: 3,
    PULSAR: 4,
    BASE: 5,
    NOPE: 6
  };

  const coordFunc = d => [d.x, d.y];

  let $replay = $('#replay'),
    $playPause = $('#replay-playpause'),
    $previous = $('#replay-previous'),
    $next = $('#replay-next'),
    $turnLabel = $('#replay-turn-label'),
    $turnSlider = $('#replay-turn-slider');

  let playing = false, playerColors = {}, animationEnabled = true;
  let turnIndex, turnForward, playingTimer, nextTurnCallback;

  $replay.hide();

  function svg_translate(d) {
    return 'translate(' + [(d.x + .5) * CELL_SIZE, (d.y + .5) * CELL_SIZE] + ')';
  }

  // setup game board
  let svg = d3.select('#replay-board').append('svg')
    .attr('width', (2 + N_CELLS) * CELL_SIZE)
    .attr('height', (2 + N_CELLS) * CELL_SIZE);

  let defs = svg.append('defs');
  // base pattern (slashed yellow)
  let basePat = defs.append('pattern')
    .attr('id', 'pattern-base')
    .attr('patternUnits', 'userSpaceOnUse')
    .attr('width', 10).attr('height', 10);
  basePat.append('rect')
    .attr('fill', 'darkgoldenrod')
    .attr('width', 10).attr('height', 10);
  basePat.append('path')
    .attr('stroke', '#333').attr('stroke-width', 4)
    .attr('d', 'M-1,1 l2,-2 M0,10 l10,-10 M9,11 l2,-2');

  // grid pattern (tiles)
  let gridPat = defs.append('pattern')
    .attr('id', 'pattern-grid')
    .attr('patternUnits', 'userSpaceOnUse')
    .attr('patternTransform', 'translate(' + [CELL_SIZE / 2, CELL_SIZE / 2] + ')')
    .attr('shape-rendering', 'crispEdges')
    .attr('width', CELL_SIZE * 2).attr('height', CELL_SIZE * 2);
  gridPat.append('rect')
    .attr('x', CELL_SIZE).attr('y', 0)
    .attr('width', CELL_SIZE).attr('height', CELL_SIZE)
    .attr('fill', 'rgba(255,255,255,.1)');
  gridPat.append('rect')
    .attr('x', 0).attr('y', CELL_SIZE)
    .attr('width', CELL_SIZE).attr('height', CELL_SIZE)
    .attr('fill', 'rgba(255,255,255,.1)');

  defs.append('clipPath')
    .attr('id', 'clip')
    .append('rect')
    .attr('x', 0).attr('y', 0)
    .attr('width', 32).attr('height', 32);

  // pipe sprite
  let spriteScale = CELL_SIZE / 32;
  // pipe, super-pipe
  for (let [typeN, typeName] of [[0, 'p'], [1, 'sp']]) {
    for (let i = 0; i < 20; i++) {
      defs
        .append('symbol')
        .attr('id', typeName + i)
        .append('image')
        .attr('transform', 'translate(-' + i * 32 + ' -' + typeN * 32 + ')')
        .attr('xlink:href', '/static/pipes.png')
        // .attr('clip-path', 'url(#clip)')
        .attr('width', 32 * 20).attr('height', 32 * 2);
    }
  }

  // explosion sprite
  for (let i = 0; i < 16; i++) {
    defs
      .append('symbol')
      .attr('id', 'explosion' + i)
      .append('image')
      .attr('transform', 'translate(-' + (i % 4) * 32 + ' -' + parseInt(i / 4) * 32 + ')')
      .attr('xlink:href', '/static/boom.png')
      // .attr('clip-path', 'url(#clip)')
      .attr('width', 32 * 4).attr('height', 32 * 4);
  }

  // pulsar sprite
  defs
    .append('symbol')
    .attr('id', 'pulsar')
    .append('image')
    .attr('xlink:href', '/static/energy.gif')
    .attr('width', 64).attr('height', 64);

  // debris sprite
  defs
    .append('symbol')
    .attr('id', 'debris')
    .append('image')
    .attr('xlink:href', '/static/debris.png')
    .attr('width', 32).attr('height', 32);

  let svgContent = svg.append('g')
    .attr('transform', 'translate(' + [CELL_SIZE / 2, CELL_SIZE / 2] + ')');

  // grid surface
  svgContent.append('rect')
    .attr('shape-rendering', 'crispEdges')
    .attr('x', CELL_SIZE * 1.5).attr('y', CELL_SIZE * 1.5)
    .attr('width', (N_CELLS - 2) * CELL_SIZE).attr('height', (N_CELLS - 2) * CELL_SIZE)
    .attr('fill', 'url(#pattern-grid)');

  // layers
  let baseLayer = svgContent.append('g');
  let plasmaLayer = svgContent.append('g');
  let pipeLayer = svgContent.append('g');

  let baseCenter = Math.floor(N_CELLS / 2 - N_BASES / 2);
  let base = (x, y, w, h) => baseLayer
    .append('rect')
    .attr('class', 'base')
    .attr('width', w * CELL_SIZE)
    .attr('height', h * CELL_SIZE)
    .attr('shape-rendering', 'crispEdges')
    .attr('fill', 'url(#pattern-base)')
    .attr('transform', svg_translate({x: x, y: y}));
  let basePipes = (x, y, dx, dy, frame) => {
    for (let i = 0; i < N_BASES; i++) {
      baseLayer
        .append('g')
        .attr('class', 'base-pipe')
        .attr('transform', svg_translate({x: x + dx * i, y: y + dy * i}))
        .append('use')
        .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale)
        .attr('transform', 'scale(' + spriteScale + ')')
        .attr('xlink:href', '#p' + frame);
    }
  };
  base(baseCenter - B_PAD / 2, -1 - B_OFFSET, N_BASES + B_PAD, B_WIDTH);
  base(baseCenter - B_PAD / 2, N_CELLS - 1 + B_OFFSET, N_BASES + B_PAD, B_WIDTH);
  base(-1 - B_OFFSET, baseCenter - B_PAD / 2, B_WIDTH, N_BASES + B_PAD);
  base(N_CELLS - 1 + B_OFFSET, baseCenter - B_PAD / 2, B_WIDTH, N_BASES + B_OFFSET);
  basePipes(baseCenter, 0, 1, 0, 18);
  basePipes(baseCenter, N_CELLS - 1, 1, 0, 16);
  basePipes(0, baseCenter, 0, 1, 17);
  basePipes(N_CELLS - 1, baseCenter, 0, 1, 19);

  function renderMap(previousMapData, mapData, pulsarData) {
    let index = (x, y) => x * N_CELLS + y;
    let plasmaAnimated = animationEnabled && turnForward && previousMapData;
    let explosionData = [];
    let clearedDebrisData = [];

    function spriteIndex(d) {
      return DIRECTIONS.reduce((acc, dir, i) => {
        let ind = index(d.x + dir.x, d.y + dir.y);
        let o = mapData[ind];
        if (!o)
          return acc;
        return acc + (
            o.type === CellType.PIPE ||
            o.type === CellType.SUPER_PIPE ||
            o.type === CellType.BASE ||
            o.type === CellType.PULSAR ? 1 << i : 0);
      }, 0);
    }

    // render base vacuum force
    let baseData = mapData.filter(d => d.type === CellType.BASE);

    let bases = svgContent.selectAll('g.base').data(baseData, coordFunc);
    let base = bases.enter()
      .append('g').attr('class', 'base')
      .attr('transform', d => svg_translate({
        x: d.x + (baseCenter <= d.y && d.y <= baseCenter + N_BASES ? 1 : 0) * (d.x > N_CELLS / 2 ? 1 : -1),
        y: d.y + (baseCenter <= d.x && d.x <= baseCenter + N_BASES ? 1 : 0) * (d.y > N_CELLS / 2 ? 1 : -1)
      }));
    base.append('circle')
      .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2)
      .attr('r', CELL_SIZE / 2 * .9)
      .attr('fill', d => playerColors[d.owner]);
    base.append('text')
      .attr('x', CELL_SIZE / 2).attr('y', CELL_SIZE / 1.5)
      .attr('font-size', CELL_SIZE * .5 + 'px')
      .attr('fill', 'black')
      .attr('text-anchor', 'middle');
    bases.select('text').text(d => d.vacuum);

    // build pipe clusters & vacuum graph
    for (let pipe of mapData) {
      if (!(pipe.type === CellType.PIPE || pipe.type === CellType.SUPER_PIPE))
        continue;
      pipe.baseDistance = Infinity;
      pipe.cluster = null;
      pipe.baseDistanceScale = [Infinity, -Infinity];
    }

    let queue = new PriorityQueue({
      comparator: (a, b) => a[0] - b[0]
    });
    let stack = [], cluster = 0;
    for (let i = 0; i < mapData.length; i++) {
      if (mapData[i].type !== CellType.BASE)
        continue;
      mapData[i].baseDistance = -mapData[i].vacuum;
      queue.queue([-mapData[i].vacuum, mapData[i].x, mapData[i].y]);
      stack.push([i, cluster++]);
    }

    while (queue.length) {
      let [vacuum, x, y] = queue.dequeue();
      for (let dir of DIRECTIONS) {
        let xx = x + dir.x, yy = y + dir.y;
        let i = index(xx, yy);
        let cell = mapData[i];
        if (!cell)
          continue;
        if (!(cell.type === CellType.PIPE || cell.type === CellType.SUPER_PIPE))
          continue;
        if (cell.baseDistance === Infinity) {
          mapData[i].baseDistance = vacuum + 1;
          queue.queue([vacuum + 1, xx, yy]);
        }
      }
    }

    let vacuumPaths = {};

    for (let i = 0; i < mapData.length; i++) {
      let pipe = mapData[i];
      if (!(pipe.type === CellType.PIPE || pipe.type === CellType.SUPER_PIPE))
        continue;
      if (pipe.baseDistance === Infinity) {
        vacuumPaths[i] = [];
        continue;
      }
      let paths = [];
      let move = (cell, factor, speed) => {
        if (speed === 0 || cell.type === CellType.BASE)
          return;
        let directions = [], lowestDistance = Infinity;
        for (let dir of DIRECTIONS) {
          let other = mapData[index(cell.x + dir.x, cell.y + dir.y)];
          if (!other || !(other.type === CellType.PIPE || other.type === CellType.SUPER_PIPE || other.type === CellType.BASE))
            continue;
          if (other.baseDistance !== Infinity) {
            if (other.baseDistance < lowestDistance) {
              directions = [dir];
              lowestDistance = other.baseDistance;
            } else if (other.baseDistance === lowestDistance) {
              directions.push(dir);
            }
          }
        }
        if (directions.length) {
          for (let dir of directions) {
            let other = mapData[index(cell.x + dir.x, cell.y + dir.y)];
            paths.push({
              from: {x: cell.x, y: cell.y},
              to: {x: other.x, y: other.y},
              toBase: other.type === CellType.BASE,
              factor: factor / directions.length,
              speed: speed
            });
            move(other, factor / directions.length, speed - 1);
          }
        }
      };
      move(pipe, 1, pipe.type === CellType.SUPER_PIPE ? 2 : 1);
      vacuumPaths[i] = paths;
    }

    // pulsar loot
    for (let i = 0; i < pulsarData.length; i++) {
      let pulsar = pulsarData[i];
      let paths = [];
      for (let dir of DIRECTIONS) {
        let cell = mapData[index(pulsar.x + dir.x, pulsar.y + dir.y)];
        if (!cell || !(cell.type === CellType.PIPE || cell.type === CellType.SUPER_PIPE))
          continue;
        if (cell.baseDistance !== Infinity) {
          paths.push({x: cell.x, y: cell.y});
        }
      }
      pulsarData[i].energyPaths = paths;
    }

    let pulsars = svgContent.selectAll('g.pulsar').data(pulsarData, coordFunc);
    pulsars.enter()
      .append('g').attr('class', 'pulsar')
      .attr('transform', svg_translate)
      .append('use')
      .attr('xlink:href', '#pulsar')
      .attr('transform', 'scale(' + spriteScale / 2 + ')')
      .attr('width', CELL_SIZE / spriteScale * 2).attr('height', CELL_SIZE / spriteScale * 2);

    pulsars.exit().remove();
    pulsars.select('use')
      .attr('opacity', d => d.n_pulses > 0 ? 1 : .3);

    if (turnForward) {
      let pulsating = pulsarData.filter(d => d.n_pulses > 0 && turnIndex > 0 && (turnIndex + 1) % d.period === 0);

      if (animationEnabled) {
        let pulsations = svgContent.selectAll('g.pulsar').data(pulsating, coordFunc);
        pulsations
          .append('circle')
          .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2)
          .attr('r', CELL_SIZE / 3)
          .attr('stroke', 'white')
          .attr('stroke-width', 1.5)
          .attr('opacity', 1)
          .transition().ease('linear').duration(TURN_DURATION)
          .attr('r', CELL_SIZE * 1.5)
          .attr('opacity', 0)
          .transition().duration(0).remove();

        // pulsar loot
        for (let pulsar of pulsarData) {
          if (pulsar.n_pulses <= 0 || turnIndex % pulsar.period !== 0)
            continue;
          for (let destination of pulsar.energyPaths) {
            plasmaLayer.append('circle')
              .attr('fill', 'white')
              .attr('fill-opacity', .9)
              .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2)
              .attr('r', Math.max(0.1, Math.min(0.9, pulsar.power / 2)) * CELL_SIZE / 3)
              .attr('transform', svg_translate(pulsar))
              .transition().ease('linear').duration(TURN_DURATION)
              .attr('transform', svg_translate(destination))
              .transition().duration(0).remove();
          }
        }
      }
    }

    if (plasmaAnimated) {
      // fancy plasma animation

      let energyData = previousMapData
        .filter(d => (d.type === CellType.PIPE || d.type === CellType.SUPER_PIPE) && d.plasma > 0)
        .map(d => ({
          x: d.x,
          y: d.y,
          plasma: d.plasma,
          speed: d.type === CellType.SUPER_PIPE ? 2 : 1,
          paths: vacuumPaths['' + index(d.x, d.y)]
        }));

      energyData.map(d => {
        if (!d.paths) return;
        d.paths.map(p => {
          let duration = TURN_DURATION / d.speed;
          let offset = d.speed - p.speed;
          let animate = () => {
            let t = plasmaLayer
              .append('circle')
              .attr('r', Math.max(0.1, Math.min(0.9, d.plasma * p.factor / 2)) * CELL_SIZE / 3)
              .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2)
              .attr('transform', svg_translate(p.from))
              .attr('fill', 'white')
              .attr('fill-opacity', .9)
              .transition().ease('linear').duration(duration)
              .attr('transform', svg_translate(p.to));
            if (p.toBase)
            // fade out
              t = t.attr('fill-opacity', .6)
                .transition().duration(TURN_DURATION)
                .attr('fill-opacity', 0).attr('r', .1);
            t.transition().duration(0).remove();
          };
          if (offset) setTimeout(animate, offset * duration);
          else animate();
        });
      });
      // clear previous plasma
      pipeLayer.selectAll('g.pipe circle').attr('fill', 'none');
    }

    // render the pipes
    let pipeData = mapData.filter(d => d.type === CellType.PIPE || d.type === CellType.SUPER_PIPE);
    let pipes = pipeLayer.selectAll('g.pipe').data(pipeData, coordFunc);

    let pipe = pipes.enter()
      .append('g').attr('class', 'pipe')
      .attr('transform', svg_translate);
    pipe
      .append('circle')
      .attr('fill-opacity', .9)
      .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2);
    let tt = pipe
      .append('use')
      .attr('transform', 'scale(' + spriteScale + ')')
      .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale)
      .attr('opacity', 1);
    if (animationEnabled)
      tt.attr('opacity', 0).transition().duration(TURN_DURATION).attr('opacity', 1);

    pipes.exit().each(d => {
      // explode animation building
      explosionData.push(d);
    }).remove();
    pipes.select('use')
      .attr('xlink:href', d => '#' + (d.type === CellType.SUPER_PIPE ? 'sp' : 'p') + spriteIndex(d));

    tt = pipes.select('circle')
    if (plasmaAnimated)
      tt = tt.transition().delay(TURN_DURATION).duration(0);
    tt.attr('fill', d => d.plasma > 0 ? 'white' : 'none')
      .attr('r', d => Math.max(0.1, Math.min(0.9, d.plasma / 2)) * CELL_SIZE / 3);

    let debrisData = mapData.filter(d => d.type === CellType.DEBRIS);

    let debris = svgContent.selectAll('g.debris').data(debrisData, coordFunc);
    debris.enter()
      .append('g').attr('class', 'debris')
      .attr('transform', svg_translate)
      .append('use')
      .attr('xlink:href', '#debris')
      .attr('transform', 'scale(' + spriteScale + ')')
      .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale);

    debris.exit().each(d => {
      // clear animation building
      clearedDebrisData.push(d);
    }).remove();

    if (animationEnabled && turnForward && clearedDebrisData.length) {
      svgContent.selectAll('circle.clearing').data(clearedDebrisData, coordFunc)
        .enter()
        .append('circle')
        .attr('class', 'clearing')
        .attr('transform', svg_translate)
        .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2)
        .attr('r', CELL_SIZE / 3)
        .attr('stroke', 'goldenrod')
        .attr('stroke-width', 1.5)
        .attr('opacity', 1)
        .transition().ease('linear').duration(TURN_DURATION)
        .attr('r', CELL_SIZE * 1.5)
        .attr('opacity', 0)
        .transition().duration(0).remove();
    }

    if (animationEnabled && turnForward && explosionData.length) {
      svgContent.selectAll('.explosion').data(explosionData, coordFunc)
        .enter()
        .append('use')
        .attr('class', 'explosion')
        .attr('xlink:href', '#explosion0')
        .attr('transform', d => svg_translate(d) + ' scale(' + spriteScale + ')')
        .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale)
        .transition().duration(TURN_DURATION)
        .tween("xlink:href", function () {
          let i = d3.interpolateRound(0, 15);
          return function (t) {
            this.setAttribute('href', '#explosion' + i(t));
          };
        })
        .transition().duration(0).remove();
    }
  }

  function renderTurn(previousTurnData, turnData) {
    let complete = (d, i) => ({
      // literal copy is *much* faster than $.extend
      owner: d.owner,
      plasma: d.plasma,
      vacuum: d.vacuum,
      type: parseInt(d.type),
      x: parseInt(i / N_CELLS),
      y: i % N_CELLS
    });
    for (let pid in turnData.players) {
      if (!turnData.players.hasOwnProperty(pid))
        continue;
      // update scores
      $('.replay-player[data-id=' + pid + '] .score').text(turnData.players[pid].score);
    }
    renderMap(previousTurnData ? previousTurnData.map.map(complete) : null, turnData.map.map(complete), turnData.pulsars);
    if (playing) {
      clearTimeout(playingTimer);
      playingTimer = setTimeout(nextTurnCallback, TURN_DURATION + 25);
    }
  }

  console.log('loading dump');
  $.get("dump/", function (game_data) {
    let turns = game_data.split('\n').filter(Boolean).map(JSON.parse);
    console.log('loaded', turns.length, 'turns');
    let firstTurn = turns[0];
    let turnCount = firstTurn.turn[1];

    // build player legend
    let $legend = $('#replay-legend').empty();
    let i = 0;
    for (let pid in firstTurn.players) {
      if (firstTurn.players.hasOwnProperty(pid)) {
        let player = firstTurn.players[pid], color = PLAYER_COLORS[i++];
        playerColors[pid] = color;
        let $el = $('<div/>').attr('data-id', pid).addClass('replay-player');
        let svg = d3.select($el[0]).append('svg').attr('width', CELL_SIZE).attr('height', CELL_SIZE);
        svg.append('circle').attr('r', CELL_SIZE / 2).attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2).attr('fill', color);
        $el
          .append($('<span/>').addClass('name').text(player.name))
          .append($('<span/>').addClass('score'));
        $legend.append($el);
      }
    }

    // ui handlers
    $previous.click(function () {
      turnIndex--;
      turnForward = false;
      $turnSlider.val(turnIndex).trigger('change');
    });

    $next.click(function () {
      turnIndex++;
      turnForward = true;
      $turnSlider.val(turnIndex).trigger('change');
    });

    $turnSlider.change(function () {
      let newTurnIndex = parseInt($turnSlider.val());
      if (turnIndex != newTurnIndex)
        turnForward = turnIndex < newTurnIndex;
      turnIndex = newTurnIndex;
      $previous.prop('disabled', turnIndex <= 0);
      $next.prop('disabled', turnIndex >= turns.length - 1);
      $turnLabel.text(('000' + turns[turnIndex].turn[0]).slice(-3));
      renderTurn(turns[turnIndex - 1], turns[turnIndex]);
    });
    $turnSlider.attr('min', 0).attr('max', turns.length - 1).val(0);
    $turnSlider.trigger('change');

    nextTurnCallback = function () {
      clearTimeout(playingTimer); // just to be sure
      turnIndex++;
      turnForward = true;
      if (turnIndex >= turns.length) {
        turnForward = false;
        turnIndex = turns.length - 1;
        playing = true; // trigger pause
        $playPause.trigger('click');
        return;
      }
      $turnSlider.val(turnIndex).trigger('change');
    };

    $playPause.click(function () {
      if (!playing && turnIndex >= turns.length - 1)
        return;
      playing = !playing;
      $playPause
        .find('span').text(playing ? 'Pause' : 'Lecture');
      $playPause
        .find('i')
        .toggleClass('fa-play', !playing)
        .toggleClass('fa-pause', playing);
      if (playing) {
        playingTimer = setTimeout(nextTurnCallback, TURN_DURATION);
      } else {
        clearTimeout(playingTimer);
      }
    });

    $('body').keydown(function (e) {
      function stop() {
        e.preventDefault();
        e.stopPropagation();
      }

      let key = e.which, offset = e.shiftKey ? 10 : 1;
      if (key === 32) {
        // space
        stop();
        $playPause.trigger('click');
      } else if (key === 37) {
        // left
        stop();
        turnIndex -= offset;
        $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 39) {
        // right
        stop();
        turnIndex += offset;
        $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 65) {
        // a
        stop();
        turnIndex = 0;
        $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 69) {
        // e
        stop();
        turnIndex = turns.length - 1;
        $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 77) {
        // m
        animationEnabled = !animationEnabled;
      }
    });

    // ready, trigger a fake play/pause to init everything
    playing = true;
    $playPause.trigger('click');

    // reveal the UI
    $replay.fadeIn('fast');
  });
});
