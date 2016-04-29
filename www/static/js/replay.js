$(function () {
  const N_CELLS = 39,
    CELL_SIZE = 16,
    N_BASES = parseInt(N_CELLS / 3),
    TURN_DURATION = 300;

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

  let playing = false, turnIndex, turnForward, playingTimer;

  $replay.hide();

  function svg_translate(d) {
    return 'translate(' + [(d.x + .5) * CELL_SIZE, (d.y + .5) * CELL_SIZE] + ')';
  }

  // setup game board
  var svg = d3.select('#replay-board').append('svg')
    .attr('width', (1 + N_CELLS) * CELL_SIZE)
    .attr('height', (1 + N_CELLS) * CELL_SIZE);

  let defs = svg.append('defs');
  // base pattern (slashed yellow)
  let basePat = defs.append('pattern')
    .attr('id', 'pattern-base')
    .attr('patternUnits', 'userSpaceOnUse')
    .attr('width', 10).attr('height', 10);
  basePat.append('rect')
    .attr('fill', 'goldenrod')
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
    for (let i = 0; i < 16; i++) {
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

  // explode sprite
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

  // grid surface
  svg.append('rect')
    .attr('x', CELL_SIZE * 1.5).attr('y', CELL_SIZE * 1.5)
    .attr('width', (N_CELLS - 2) * CELL_SIZE).attr('height', (N_CELLS - 2) * CELL_SIZE)
    .attr('fill', 'url(#pattern-grid)');

  let svgContent = svg.append('g');

  let baseCenter = Math.floor(N_CELLS / 2 - N_BASES / 2);
  let base = (x, y, w, h) => svgContent
    .append('rect')
    .attr('class', 'base')
    .attr('width', w * CELL_SIZE)
    .attr('height', h * CELL_SIZE)
    .attr('shape-rendering', 'crispEdges')
    .attr('fill', 'url(#pattern-base)')
    .attr('transform', svg_translate({x: x, y: y}));
  base(baseCenter, -1, N_BASES, 2);
  base(baseCenter, N_CELLS - 1, N_BASES, 2);
  base(-1, baseCenter, 2, N_BASES);
  base(N_CELLS - 1, baseCenter, 2, N_BASES);

  function renderPulsars(pulsarData) {
    // n_pulses period power x y
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

    if (!turnForward)
      return;
    let pulsating = pulsarData.filter(d => d.n_pulses > 0 && turnIndex > 0 && (turnIndex + 1) % d.period === 0);
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
  }

  function renderMap(mapData) {
    let index = (x, y) => x * N_CELLS + y;

    function spriteIndex(d) {
      return DIRECTIONS.reduce((acc, dir, i) => {
        var ind = index(d.x + dir.x, d.y + dir.y);
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

    let explosionData = [];
    let clearedDebrisData = [];

    let pipeData = mapData.filter(d => d.type === CellType.PIPE || d.type === CellType.SUPER_PIPE);

    let pipes = svgContent.selectAll('g.pipe').data(pipeData, coordFunc);
    let pipe = pipes.enter()
      .append('g').attr('class', 'pipe')
      .attr('transform', svg_translate);
    pipe.append('circle')
      .attr('fill-opacity', '.9')
      .attr('cx', CELL_SIZE / 2).attr('cy', CELL_SIZE / 2);
    pipe.append('use')
      .attr('transform', 'scale(' + spriteScale + ')')
      .attr('width', CELL_SIZE / spriteScale).attr('height', CELL_SIZE / spriteScale);
    pipes.exit().each(d => {
      explosionData.push(d);
    }).remove();
    pipes.select('use')
      .attr('xlink:href', d => '#' + (d.type === CellType.SUPER_PIPE ? 'sp' : 'p') + spriteIndex(d));
    pipes.select('circle')
      .attr('fill', d => d.plasma > 0 ? 'white' : 'none')
      .attr('r', d => Math.max(0.1, Math.min(0.9, d.plasma)) * CELL_SIZE / 3);

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
      clearedDebrisData.push(d);
    }).remove();

    if (turnForward && clearedDebrisData.length) {
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

    if (turnForward && explosionData.length) {
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

  function renderTurn(turnData) {
    renderPulsars(turnData.pulsars);
    renderMap(turnData.map.map((d, i) => ({
      // literal copy is *much* faster than $.extend
      owner: d.owner,
      plasma: d.plasma,
      vacuum: d.vacuum,
      type: parseInt(d.type),
      x: parseInt(i / N_CELLS),
      y: i % N_CELLS
    })));
  }

  console.log('loading dump');
  $.get("dump/", function (game_data) {
    let turns = game_data.split('\n').filter(Boolean).map(JSON.parse);
    console.log('loaded', turns.length, 'turns');
    let firstTurn = turns[0];
    let turnCount = firstTurn.turn[1];

    // TODO: player scores & stuff

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
      $turnLabel.text(turns[turnIndex].turn[0] + ' (' + turnIndex + ')');
      renderTurn(turns[turnIndex]);
    });
    $turnSlider.attr('min', 0).attr('max', turns.length - 1).val(0);
    $turnSlider.trigger('change');

    $playPause.click(function () {
      playing = !playing;
      $playPause
        .find('span').text(playing ? 'Pause' : 'Lecture');
      $playPause
        .find('i')
        .toggleClass('fa-play', !playing)
        .toggleClass('fa-pause', playing);
      if (playing) {
        playingTimer = setInterval(function () {
          turnIndex++;
          turnForward = true;
          if (turnIndex >= turns.length) {
            turnForward = false;
            // turnIndex = turns.length - 1;
            turnIndex = 0;
            // clearInterval(playingTimer); // just to be sure
            // playing = true; // trigger pause
            // $playPause.trigger('click');
            return;
          }
          $turnSlider.val(turnIndex).trigger('change');
        }, TURN_DURATION);
      } else {
        clearInterval(playingTimer);
      }
    });

    $('body').keydown(function (e) {
      function stop() {
        e.preventDefault();
        e.stopPropagation();
      }

      var key = e.which,
        repeat = e.shiftKey ? 10 : 1,
        i;
      if (key === 32) {
        // space
        stop();
        $playPause.trigger('click');
      } else if (key == 37) {
        // left
        stop();
        for (i = 0; i < repeat; i++)
          $previous.trigger('click');
      } else if (key == 39) {
        // left
        stop();
        for (i = 0; i < repeat; i++)
          $next.trigger('click');
      } else if (key == 65) {
        // a
        stop();
        turnIndex = 0;
        $turnSlider.val(turnIndex).trigger('change');
      } else if (key == 69) {
        // e
        stop();
        turnIndex = turns.length - 1;
        $turnSlider.val(turnIndex).trigger('change');
      }
    });

    // ready, trigger a fake playpause to init everything
    playing = true;
    $playPause.trigger('click');

    // reveal the UI
    $replay.fadeIn();
  });

});