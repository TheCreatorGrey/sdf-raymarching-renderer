class array3 {
    constructor(sizeX, sizeY, sizeZ, initial=null) {
        this.size = {
            x:sizeX,
            y:sizeY,
            z:sizeZ
        }

        let layers = []
        for (let z = 1; z <= this.size.z; z++) {
            let slab = []
            for (let y = 1; y <= this.size.y; y++) {
                let rod = []
                for (let x = 1; x <= this.size.x; x++) {
                    rod.push(initial);
                }
                slab.push(rod)
            }
            layers.push(slab)
        }
        this.arr = layers
    }

    find(x, y, z) {
        if (z in this.arr) {
            if (y in this.arr[z]) {
                if (x in this.arr[z][y]) {
                    return this.arr[z][y][x]
                }
            }
        }
        return null
    }

    set(x, y, z, value) {
        this.arr[z][y][x] = value
    }

    iter(f) {
        let z = 0;
        for (let layer of this.arr) {
            let y = 0;
            for (let rod of layer) {
                let x = 0;
                for (let item of rod) {
                    f(x, y, z, item)

                    x += 1
                }
                y += 1
            }
            z += 1
        }
    }
}