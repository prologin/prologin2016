$(function () {

  const N_CELLS = 39, CELL_SIZE = 14.5, N_BASES = parseInt(N_CELLS / 3);

  let svg_translate = (d) => 'translate(' + [(d.x + .5) * CELL_SIZE, (d.y + .5) * CELL_SIZE] + ')';

  let $controls;
  $('#map-preview').append($controls = $('<form id="map-controls"/>'));

  let svg = d3.select('#map-preview').append('svg')
    .attr('width', (N_CELLS + 1) * CELL_SIZE)
    .attr('height', (N_CELLS + 1) * CELL_SIZE)

  let defs = svg.append('defs');
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

  svg.append('rect')
    .attr('width', svg.attr('width'))
    .attr('height', svg.attr('height'))
    .attr('fill', 'black');

  svg.append('rect')
    .attr('x', CELL_SIZE * 1.5).attr('y', CELL_SIZE * 1.5)
    .attr('width', (N_CELLS - 2) * CELL_SIZE).attr('height', (N_CELLS - 2) * CELL_SIZE)
    .attr('fill', 'url(#pattern-grid)');

  let svg_content = svg.append('g').attr('id', 'content');

  let $mapContents = $('#map-contents');
  let mapLines = $mapContents.text().split('\n');
  $mapContents.empty().text();
  for (let line of mapLines)
    $mapContents.append($('<span/>').text(line)).append('\n');

  let periodMin = Infinity, periodMax = -Infinity,
    powerMin = Infinity, powerMax = -Infinity,
    pulsesMin = Infinity, pulsesMax = -Infinity;
  let pulsars = mapLines.map(line => line.split(/\s/).map(e => parseInt(e))).map((p, line) => {
    let [x, y, period, power, pulses] = p;
    periodMin = Math.min(periodMin, period);
    periodMax = Math.max(periodMax, period);
    powerMin = Math.min(powerMin, power);
    powerMax = Math.max(powerMax, power);
    pulsesMin = Math.min(pulsesMin, pulses);
    pulsesMax = Math.max(pulsesMax, pulses);
    return {
      x: x,
      y: y,
      period: period,
      power: power,
      pulses: pulses,
      line: line
    };
  });

  let periodScale = d3.scale.linear().domain([periodMin, periodMax]).range(["green", "red"]);
  let powerScale = d3.scale.linear().domain([powerMin, powerMax]).range(["red", "green"]);
  let pulsesScale = d3.scale.linear().domain([pulsesMin, pulsesMax]).range(["red", "green"]);

  let scales = {period: periodScale, power: powerScale, pulses: pulsesScale};
  let scaleNames = {
    period: ["Période", "courte", "longue"],
    power: ["Puissance", "peu chargé", "très chargé"],
    pulses: ["Capacité", "peu de pulses", "beaucoup de pulses"],
  };

  for (let prop in scaleNames) {
    let [name, minText, maxText] = scaleNames[prop], id = "map-prop-type-" + prop;
    $controls
      .append($('<input type="radio" name="map-prop-type">')
        .attr('value', prop)
        .attr('id', id)
        .on('change', e => drawPulsars(prop)))
      .append($('<label/>')
        .text(name + ' (').attr('for', id)
        .append($('<span/>').css('color', scales[prop](-9999)).text(minText))
        .append(', ')
        .append($('<span/>').css('color', scales[prop](9999)).text(maxText))
        .append(')'));
  }

  let baseCenter = Math.floor(N_CELLS / 2 - N_BASES / 2);
  let base = (x, y, w, h) => svg_content
    .append('rect')
    .attr('class', 'base')
    .attr('width', w * CELL_SIZE)
    .attr('height', h * CELL_SIZE)
    .attr('shape-rendering', 'crispEdges')
    .attr('fill', 'url(#pattern-base)')
    .attr('transform', svg_translate({x: x, y: y}));
  base(baseCenter, 0, N_BASES, 1);
  base(baseCenter, N_CELLS - 1, N_BASES, 1);
  base(0, baseCenter, 1, N_BASES);
  base(N_CELLS - 1, baseCenter, 1, N_BASES);

  let svgPulsars = svg_content.selectAll('g.pulsar').data(pulsars);
  svgPulsars
    .enter()
    .append('g')
    .attr('class', 'pulsar')
    .attr('transform', svg_translate)
    .append('circle')
    .attr('cx', CELL_SIZE / 2)
    .attr('cy', CELL_SIZE / 2)
    .attr('r', CELL_SIZE / 3)
  svgPulsars.exit().remove();

  let overlay = svg.append('g');

  function drawPulsars(prop) {

    let infoToggle = function (shown) {
      return function () {
        let that = d3.select(this);
        that.select('circle')
          .attr('r', shown ? CELL_SIZE / 1.5 : CELL_SIZE / 3)
          .attr('stroke', shown ? 'white' : 'none');
        console.log([that.data()]);
        let t = overlay.selectAll('text').data(shown ? that.data() : []);
        $mapContents.find('>span').removeClass('hl');
        if (shown)
          $mapContents.find('>span:eq(' + that.data()[0].line + ')').addClass('hl');
        t.enter()
          .append('text')
          .attr('text-anchor', 'middle')
          .attr('transform', that.attr('transform'))
          .attr('y', -CELL_SIZE)
          .attr('fill', 'white')
          .text(d => '(' + d.x + ', ' + d.y + ') ⋅ ' + scaleNames[prop][0].toLowerCase() + ' = ' + d[prop]);
        t.exit().remove();
      };
    };

    svgPulsars
      .on('mouseover', infoToggle(true))
      .on('mouseout', infoToggle(false))
      .select('circle')
      .transition().duration(500)
      .attr('fill', d => scales[prop](d[prop]))
  }

  $controls.on('change', e => {
    drawPulsars($('input[type=radio]:checked', $controls).val());
  });

  $('#map-prop-type-period').trigger('click');

});