$(function () {
  let logo = [7, 11, 13, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 8, 11, 10, 7, 11, 9, 7, 11, 13, 6, 7, 11, 13, 7, 11, 13, 6, 7, 11, 13, 6, 0, 0, 6, 0, 0, 6, 0, 6, 6, 6, 0, 6, 6, 0, 6, 6, 6, 0, 6, 6, 0, 0, 6, 0, 0, 4, 11, 10, 6, 4, 11, 10, 4, 11, 14, 6, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 11, 10, 0, 0, 0, 0];
  let lw = 20, lh = 7, ls = .5;

  let svg = d3.select('.navbar-brand').append('svg');
  let defs = svg.append('defs');
  for (let i = 0; i < 15; i++) {
    defs
      .append('symbol')
      .attr('id', 'p' + i)
      .append('image')
      .attr('transform', 'translate(-' + i * 32 + ' 0)')
      .attr('xlink:href', '/static/pipes.png')
      .attr('width', 32 * 20)
      .attr('height', 32 * 2);
  }

  let group = svg.append('g')
    .attr('transform', 'scale(.33)')
  let tiles = group.selectAll('use').data(logo.map((e, i) => ({
    x: i % lw,
    y: parseInt(i / lw),
    e: e - 1
  })).filter(d => d.e != -1));
  tiles.enter().append('use')
    .attr('x', d => d.x * 32)
    .attr('y', d => d.y * 32)
    .attr('xlink:href', d => '#p' + d.e)
    .attr('width', 32)
    .attr('height', 32);
});