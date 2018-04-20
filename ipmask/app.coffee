
# rotate/flip a quadrant appropriately
rot = (n, p, r)->
    if r.y == 0
        if r.x == 1
            p.x = n-1 - p.x;
            p.y = n-1 - p.y;
        [p.x, p.y] = [p.y, p.x] 
    p
    
# convert (x,y) to d
xy2d = (n, p)->
    d = 0
    r = {}
    s = n>>1
    while s>0
        r.x = (p.x & s) > 0;
        r.y = (p.y & s) > 0;
        d += s * s * ((3 * r.x) ^ r.y);
        p = rot(s, p, r);
        s = s>>1
        
    d
 
#convert d to (x,y)
d2xy = (n, d)->
    t = d
    p = {x:0, y:0}
    r = {}
    s = 1
    while s < n
        r.x = 1 & (t>>1)
        r.y = 1 & (t ^ r.x)
        p = rot(s, p, r)
        p.x += s * r.x
        p.y += s * r.y
        t = t>>2
        s = s<<1
    p

# >>>0 Трюк для перевода в uint32
cidr2range = (cidr)->
    [ip, mask] = cidr.split "/"
    console.log cidr, ip, mask
    mask = parseInt(mask, 10)  >>>0
    mask = (-1) << (32 - mask) >>>0
    ip = ip.split(".").map (a)->parseInt a, 10
    ip = ip[0]<<24 | ip[1]<<16 | ip[2]<<8 | ip[3] >>>0

    start = ip    &	 mask >>> 0
    end   = start | ~mask >>> 0
    start: start, end: end

ip2hex = (ip)-> "00000000#{ip.toString(16)}"[-8..]
ip2dec = (ip)->
    ip = ip2hex ip
    ip = ("#{parseInt(ip[i*2..i*2+1],16)}" for i in [0..3]).join "."
    
class MaskCanvas

    constructor:(@cvs, @size=0)->
        @ctx = @cvs.getContext "2d"
        @h = @w = @cvs.height = @cvs.width = 1<<(8+@size)
        @b = cvs.getBoundingClientRect()
        @cvs.addEventListener "mousemove", @mousemove
    plot_cidr: (cidr, color) =>
        
        id = @ctx.getImageData 0, 0, @w, @h
        d = id.data
        range = cidr2range cidr
        console.log ip2hex(range.start), ip2hex(range.end)

        range.start = range.start >>2*(8-@size)>>>0
        range.end   = range.end   >>2*(8-@size)>>>0
        console.log range.start, range.end

        for l in [range.start..range.end]
            p = d2xy(@h,l)
            d[(p.x+p.y*@w)*4+0] = color.r
            d[(p.x+p.y*@w)*4+1] = color.g
            d[(p.x+p.y*@w)*4+2] = color.b
            d[(p.x+p.y*@w)*4+3] = color.a
        @ctx.putImageData id, 0, 0
    
    mousemove: (e)=>
        x = e.clientX-@b.left
        y = e.clientY-@b.top
        console.log x,y
        ip = xy2d @h, {x:x, y:y}
        console.log ip.toString(16)
        ip = ip << (8-@size)*2 >>>0
        cursor_ip.innerHTML = """#{ip2hex ip}\n#{ip2dec ip}"""

cidrs     = document.getElementById "cidrs"
cvs       = document.getElementById "cvs"
cursor_ip = document.getElementById "ip"

mask_cvs = new MaskCanvas(cvs, 2)
console.log cidrs.innerHTML
for cidr in cidrs.innerHTML.split "\n"
	mask_cvs.plot_cidr(cidr, {r:0, g:0, b:0, a:128})

console.log "start"

