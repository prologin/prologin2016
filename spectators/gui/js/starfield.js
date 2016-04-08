var StarField = (ctx, width, height) => {
    var STAR_COLOURS = ["#ffffff", "#ffe9c4", "#d4fbff"];

    function random(min, max) {
        return Math.round((Math.random() * max - min) + min);
    }

    function star_field(ctx, width, height, quantity) {
        let x,
            y,
            brightness,
            radius;

        // night sky
        ctx.fillStyle = "#000";
        ctx.fillRect(0, 0, width, height);

        ctx.save();

        for (let i = 0; i < quantity; i++) {
            x = Math.random() * width;
            y = Math.random() * height;
            radius = Math.random() * 1.1;
            brightness = random(80, 100) / 100;

            ctx.beginPath();
            ctx.globalAlpha = brightness;
            ctx.fillStyle = STAR_COLOURS[random(0, STAR_COLOURS.length)];
            ctx.arc(x, y, radius, 0, Math.PI * 2, true);
            ctx.fill();
            ctx.closePath();
        }
        ctx.restore();
    }

    return (quantity) => star_field(ctx, width, height, quantity);
};
