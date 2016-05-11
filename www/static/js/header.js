/*$(function () {
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
 });*/

$(function () {
  $('.navbar-brand').load('/static/logo.svg', () => {
    let svg = d3.select('.navbar-brand > svg');
    let circleLayer = svg.select('#layer3'), circle = svg.select('#circle');
    let radius = circle.attr('r');

    function popStar() {
      let r = Math.sqrt(Math.random()) * radius, theta = Math.random() * Math.PI * 2;
      let x = r * Math.cos(theta), y = r * Math.sin(theta);
      let alpha = Math.random() * 360;
      circleLayer
        .append('g')
        .attr('transform', 'translate(' + (x + parseFloat(circle.attr('cx'))) + ' ' + (y + parseFloat(circle.attr('cy'))) + ')')
        .append('path')
        .attr('fill', 'white')
        .attr('fill-opacity', 0)
        .attr('transform', 'scale(' + Math.random() * 4 / 10 + ') rotate(' + alpha + ')')
        .attr('d', 'm25,1 6,17h18l-14,11 5,17-15-10-15,10 5-17-14-11h18z')
        .transition().duration(1000 + Math.random() * 1000)
        .attr('fill-opacity', Math.random() * 0.8 + 0.2)
        .attr('transform', 'scale(' + Math.random() * 4 / 10 + ') rotate(' + alpha + ')')
        .transition().duration(1000 + Math.random() * 6000)
        .attr('fill-opacity', 0)
        .attr('transform', 'scale(' + Math.random() * 4 / 7 + ') rotate(' + alpha + ')')
        .transition().duration(0).each('end', function() { d3.select(this.parentNode).remove(); });
      console.log('heh');
      setTimeout(popStar, 1000 + Math.random() * 10 * 1000);
    }

    for (let i = 0; i < 20; i++)
      setTimeout(popStar, Math.random() * 10 * 1000);
  });
});