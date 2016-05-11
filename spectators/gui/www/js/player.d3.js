$(function () {
  const N_CELLS = 39;
  const CELL_SIZE = 20;
  const N_BASES = parseInt(N_CELLS / 3);
  let TURN_DURATION = 250;
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

  let
    $playPause = $('#ui-playpause'),
    $previous = $('#ui-previous'),
    $next = $('#ui-next'),
    $turnLabel = $('#ui-turn'),
    $maxTurnLabel = $('#ui-turn-max'),
    $turnSlider = $('#ui-turn-slider'),
    $coordsSection = $('#ui-mouse-section'),
    $coords = $('#ui-mouse-coords'),
    $cellInfo = $('#ui-cell-info'),
    $cellImg = $('#ui-cell-img'),
    $cellType = $('#ui-cell-type'),
    $cellProps = $('#ui-cell-props'),
    $spConnecting = $('#sp-connecting'),
    $spWaitingPlayer = $('#sp-waiting'),
    $spGameOver = $('#sp-gameover'),
    $previousNext = $('#ui-previous,#ui-next')
    ;

  let playing = false, playerColors = {}, animationEnabled = true;
  let turnIndex, turnForward, playingTimer, nextTurnCallback, lastCoords = {
    x: null,
    y: null
  };

  $spWaitingPlayer.hide();
  $spGameOver.hide();

  function index(x, y) {
    return x * N_CELLS + y;
  }

  function plasmaRadius(plasma) {
    return Math.max(0.1, Math.min(0.9, Math.sqrt(plasma / 2))) * CELL_SIZE / 3;
  }

  function svg_translate(d) {
    return 'translate(' + [(d.x + .5) * CELL_SIZE, (d.y + .5) * CELL_SIZE] + ')';
  }

  // setup game board
  let svg = d3.select('#main').append('svg')
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
        .attr('xlink:href', 'assets/pipes.png')
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
      .attr('xlink:href', 'assets/boom.png')
      // .attr('clip-path', 'url(#clip)')
      .attr('width', 32 * 4).attr('height', 32 * 4);
  }

  // pulsar sprite
  defs
    .append('symbol')
    .attr('id', 'pulsar')
    .append('image')
    .attr('xlink:href', 'assets/energy.gif')
    .attr('width', 64).attr('height', 64);

  // debris sprite
  defs
    .append('symbol')
    .attr('id', 'debris')
    .append('image')
    .attr('xlink:href', 'assets/debris.png')
    .attr('width', 32).attr('height', 32);

  let svgContent = svg.append('g')
    .attr('transform', 'translate(' + [CELL_SIZE / 2, CELL_SIZE / 2] + ')');

  svg
    .on('mousemove', () => {
      let [x, y] = d3.mouse(svgContent.node());
      x = parseInt((x - CELL_SIZE / 2) / CELL_SIZE);
      y = parseInt((y - CELL_SIZE / 2) / CELL_SIZE);
      if (x !== lastCoords.x || y !== lastCoords.y) {
        lastCoords = {x: x, y: y};
        if (x < 0 || y < 0 || x >= N_CELLS || y >= N_CELLS) {
          $coordsSection.css('visibility', 'hidden');
        } else {
          $coords.text(('00' + x).slice(-2) + ', ' + ('00' + y).slice(-2));
          $coordsSection.css('visibility', 'visible');
          updateCurrentCell(turnIndex);
        }
      }
    })
    .on('mouseleave', () => {
      $coordsSection.css('visibility', 'hidden');
    });

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
  base(baseCenter - B_PAD / 2, -1 - B_OFFSET, N_BASES + B_PAD, B_WIDTH);
  base(baseCenter - B_PAD / 2, N_CELLS - 1 + B_OFFSET, N_BASES + B_PAD, B_WIDTH);
  base(-1 - B_OFFSET, baseCenter - B_PAD / 2, B_WIDTH, N_BASES + B_PAD);
  base(N_CELLS - 1 + B_OFFSET, baseCenter - B_PAD / 2, B_WIDTH, N_BASES + B_PAD);

  function updateCurrentCell(turnIndex) {
    let x = lastCoords.x, y = lastCoords.y;

    function prop(name, value) {
      $cellProps.append($('<dt>').text(name));
      $cellProps.append($('<dd>').text(value));
    }

    function plural(q, p = 's', s = '') {
      return q <= 1 ? s : p;
    }

    $cellProps.empty();
    $cellInfo.css('visibility', 'visible');
    $cellImg.attr('class', '');
    let turn = turnCache[turnIndex], cell = turn.map[index(x, y)];
    if (!cell)
      return;
    let type = parseInt(cell.type);
    if (type === CellType.PIPE || type === CellType.SUPER_PIPE) {
      if (type === CellType.SUPER_PIPE) {
        $cellType.text("Super-tuyau");
        $cellImg.addClass('cell-super-pipe');
      } else {
        $cellType.text("Tuyau");
        $cellImg.addClass('cell-pipe');
      }
      prop("Débit", type === CellType.SUPER_PIPE ? 2 : 1);
      prop("Charge transportée", cell.plasma);
    } else if (type === CellType.DEBRIS) {
      $cellImg.addClass('cell-destroyed-pipe');
      $cellType.text("Débris de tuyau");
    } else if (type === CellType.PULSAR) {
      cell = turn.pulsars.filter(d => d.x === x && d.y === y)[0];
      $cellType.text("Pulsar");
      $cellImg.addClass('cell-pulsar');
      prop("Période", "" + cell.period + " tours");
      let tl = cell.period - 1 - ((turnIndex) % cell.period);
      prop("Émet", (cell.n_pulses === 0) ? "n/a" : (tl === 0) ? "maintenant" : "dans " + tl + " tour" + plural(tl));
      prop("Charge par pulse", cell.power);
      prop("Capacité", (cell.n_pulses > 0) ? cell.n_pulses + " pulse" + plural(cell.n_pulses) : "mort");
    } else if (type === CellType.BASE) {
      $cellType.text("Base");
      $cellImg.addClass('cell-base');
      prop("Force d'aspiration", cell.vacuum);
    } else {
      $cellInfo.css('visibility', 'hidden');
    }
  }

  function renderMap(turnIndex, previousMapData, mapData, pulsarData) {
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
    // base pipe sprite
    let basePipes = svgContent.selectAll('g.basepipe').data(baseData, coordFunc);
    basePipes.enter()
      .append('g')
      .attr('class', 'basepipe')
      .attr('transform', svg_translate)
      .append('use')
      .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale)
      .attr('transform', 'scale(' + spriteScale + ')');
    basePipes.selectAll('use')
      .attr('xlink:href', d => {
        let nx = d.x + (baseCenter <= d.y && d.y <= baseCenter + N_BASES ? 1 : 0) * (d.x > N_CELLS / 2 ? -1 : 1);
        let ny = d.y + (baseCenter <= d.x && d.x <= baseCenter + N_BASES ? 1 : 0) * (d.y > N_CELLS / 2 ? -1 : 1);
        let type = mapData[index(nx, ny)].type;
        if (type === CellType.PIPE || type === CellType.SUPER_PIPE || type === CellType.PULSAR)
          return '#p' + (16 + (d.y === 0 ? 2 : d.y === N_CELLS - 1 ? 0 : d.x === 0 ? 1 : 3));
        return '#p0';
      });

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
              .attr('r', plasmaRadius(pulsar.power))
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
              .attr('r', plasmaRadius(d.plasma * p.factor))
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
      .attr('r', d => plasmaRadius(d.plasma));

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

  function renderTurn(index) {
    let previousTurnData = turnCache[index - 1], turnData = turnCache[index];
    console.assert(turnData.turn[0] === index);
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
      $('.ui-player[data-id=' + pid + '] .ui-player-score').text(turnData.players[pid].score);
    }
    $turnLabel.text(index);
    $turnSlider.val(index);
    $previous.prop('disabled', !canGoBackwards || index <= 0);
    $next.prop('disabled', index >= turnCount);
    $playPause.prop('disabled', index >= turnCount);
    renderMap(index, previousTurnData ? previousTurnData.map.map(complete) : null, turnData.map.map(complete), turnData.pulsars);
    updateCurrentCell(index);
    if (playing) {
      clearTimeout(playingTimer);
      playingTimer = setTimeout(nextTurnCallback, TURN_DURATION + 25);
    }
  }

  let backoff = 1, socket, turnCount, turnCache = [], canGoBackwards = false, canSeek = false, tvMode = false;

  function init(turnData) {
    turnCount = turnData.turn[1];
    $maxTurnLabel.text(turnCount);
    $turnSlider.attr('min', 0).attr('max', turnCount).val(turnData.turn[0]);
    $turnSlider.prop('disabled', !canSeek);

    let updatePlayPause = function () {
      $playPause.html(playing ? '&#9646;&#9646;' : '&blacktriangleright;');
    };

    nextTurnCallback = function () {
      clearTimeout(playingTimer); // just to be sure
      if (turnIndex >= turnCount) {
        playing = false;
        updatePlayPause();
      } else {
        $next.trigger('click');
      }
    };

    let i = 0;
    for (let pid in turnData.players) {
      if (turnData.players.hasOwnProperty(pid)) {
        let player = turnData.players[pid], color = PLAYER_COLORS[i];
        playerColors[pid] = color;
        let $el = $('.ui-player:eq(' + i + ')').attr('data-id', pid);
        $el.find('.ui-player-marker').css('backgroundColor', color);
        $el.find('.ui-player-name').text(player.name);
        $el.find('.ui-player-score').text('?');
        i++;
      }
    }

    $turnSlider.change(function () {
      if (!canSeek)
        return;
      let index = parseInt($turnSlider.val());
      turnForward = false;
      $previousNext.prop('disabled', true);
      send('seek', {index: index});
    });

    $previous.click(function () {
      if (!canGoBackwards)
        return;
      turnForward = false;
      $previousNext.prop('disabled', true);
      // TODO: show spinner
      send('previous');
    });

    $next.click(function () {
      turnForward = true;
      $previousNext.prop('disabled', true);
      send('next');
    });

    $playPause.click(function () {
      if (!playing && turnIndex >= turnCount)
        return;
      playing = !playing;
      updatePlayPause();
      clearTimeout(playingTimer);
      if (playing) {
        playingTimer = setTimeout(nextTurnCallback, TURN_DURATION);
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
        $previous.trigger('click');
        // turnIndex -= offset;
        // $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 39) {
        // right
        stop();
        $next.trigger('click');
        // turnIndex += offset;
        // $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 65) {
        // a
        stop();
        // turnIndex = 0;
        // $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 69) {
        // e
        stop();
        // turnIndex = turns.length - 1;
        // $turnSlider.val(turnIndex).trigger('change');
      } else if (key === 77) {
        // m
        animationEnabled = !animationEnabled;
      }
    });
  }

  function connect() {
    console.log('trying to connect');
    socket = new WebSocket('ws://' + window.location.host + '/ws');
    socket.onopen = () => {
      backoff = 1;
      console.log('connected to server');
      send('hello');
    };
    socket.onerror = (e) => {
      console.warn(e);
      // check error code, don't reconnect if not needed
      backoff = Math.min(4, backoff + 1);
      setTimeout(connect, 1000 * Math.pow(1.5, backoff));
    };
    socket.onmessage = (msg) => {
      msg = JSON.parse(msg.data);
      console.log('ws >', msg);
      if (msg.c === 'whatsup') {
        turnCache = [];
        canGoBackwards = msg.canGoBackwards;
        canSeek = msg.canSeek;
        tvMode = msg.tvMode;
      } else if (msg.c === 'end') {
        // TODO: show game end
      } else if (msg.c === 'turn') {
        let turnData = msg.state;
        let needInit = !turnCache.length;
        turnIndex = turnData.turn[0];
        turnCache[turnIndex] = turnData;
        if (needInit) {
          init(turnData);
          $spConnecting.fadeOut(() => $('#main,#sidebar').fadeIn());
          if (tvMode) {
            setTimeout(() => $playPause.trigger('click'), 2 * TURN_DURATION);
          }
        }
        renderTurn(turnIndex);
      }
    };
  }

  function send(cmd, data) {
    let msg = $.extend({c: cmd}, data);
    console.log('ws <', msg);
    socket.send(JSON.stringify(msg));
  }

  connect();
});
