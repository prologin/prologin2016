function shuffle(array) {
  let counter = array.length;
  while (counter > 0) {
    let index = Math.floor(Math.random() * counter);
    counter--;
    let temp = array[counter];
    array[counter] = array[index];
    array[index] = temp;
  }
  return array;
}

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
        .transition().duration(0).each('end', function () {
        d3.select(this.parentNode).remove();
      });
      setTimeout(popStar, 1000 + Math.random() * 10 * 1000);
    }

    for (let i = 0; i < 20; i++)
      setTimeout(popStar, Math.random() * 10 * 1000);

    let letters = svg.select('#text4323').attr('style', '')
      .selectAll('path');
    let dur = [];
    for (let i = 0; i < letters.size(); i++)
      dur.push(i * 200);

    shuffle(dur);

    letters
      .style('opacity', 0)
      .transition().duration(1200).delay(function (d, i) {
      return dur[i];
    })
      .style('opacity', 1);
  });
});