var sad = {};
sad.internal = {};
sad.util = {};

sad.Point2D = SadPoint2D;
sad.Point3D = SadPoint3D;
sad.Point2I = SadPoint2I;
sad.Point3I = SadPoint3I;


sad.internal.plus = function(a, b) { return a + b; };
sad.internal.minus = function(a, b) { return a - b; };
sad.internal.multiply = function(a, b) { return a * b; };
sad.internal.divide = function(a, b) { return a / b; };


sad.internal.copyObject = function(dest, src) {
    for(var key in src) {
        if (src.hasOwnProperty(key)) {
            dest[key] = src[key];
        }
    }
}

sad.internal.makeMathFns = function(obj) {
    obj.plus = function(o) {
        return this.makeWithFn(sad.internal.plus, o);
    };

    obj.minus = function(o) {
        return this.makeWithFn(sad.internal.minus, o);
    };

    obj.multiply = function(o) {
        return this.makeWithFn(sad.internal.multiply, o);
    };

    obj.divide = function(o) {
        return this.makeWithFn(sad.internal.divide, o);
    };
};

//  sad.Point2D bindings

sad.Point2D.prototype.to3d = function() {
    return new sad.Point3D(this.x, this.y, 0);
};

sad.Point2D.prototype.to3i = function() {
    return new sad.Point3I(this.x, this.y, 0);
};

sad.Point2D.prototype.to2i = function() {
    return new sad.Point2I(this.x, this.y);
};

sad.Point2D.prototype.makeWithFn = function(fn, o) {
    return new sad.Point2D(fn(this.x, o.x), fn(this.y, o.y));
};

sad.internal.makeMathFns(sad.Point2D.prototype);

sad.Point2D.prototype.toString = function() {
    return "sad::Point2D(" + this.x + ',' + this.y + ')';
};

//  sad.Point3D bindings

sad.Point3D.prototype.to2d = function() {
    return new sad.Point2D(this.x, this.y);
};

sad.Point3D.prototype.to2i = function() {
    return new sad.Point2I(this.x, this.y);
};

sad.Point3D.prototype.to3I = function() {
    return new sad.Point3I(this.x, this.y, this.z);
};


sad.Point3D.prototype.makeWithFn = function(fn, o) {
    return new sad.Point3D(fn(this.x, o.x), fn(this.y, o.y), fn(this.z, o.z));
};

sad.internal.makeMathFns(sad.Point3D.prototype);

sad.Point3D.prototype.toString = function() {
    return "sad::Point3D(" + this.x + ',' + this.y + ',' + this.z + ')';
};

// sad.Point2I 

sad.Point2I.prototype.to2d = function() {
    return new sad.Point2D(this.x, this.y);
};

sad.Point2I.prototype.to3d = function() {
    return new sad.Point3D(this.x, this.y, 0);
};

sad.Point2I.prototype.to3i = function() {
    return new sad.Point3I(this.x, this.y, 0);
};


sad.Point2I.prototype.makeWithFn = function(fn, o) {
    return new sad.Point2I(fn(this.x, o.x), fn(this.y, o.y));
};

sad.internal.makeMathFns(sad.Point2I.prototype);

sad.Point2I.prototype.toString = function() {
    return "sad::Point2I(" + this.x + ',' + this.y  + ')';
};

// sad.Point3I

sad.Point3I.prototype.to2d = function() {
    return new sad.Point2D(this.x, this.y);
};

sad.Point3I.prototype.to2i = function() {
    return new sad.Point2I(this.x, this.y);
};

sad.Point3I.prototype.to3d = function() {
    return new sad.Point3D(this.x, this.y, this.z);
};


sad.Point3I.prototype.makeWithFn = function(fn, o) {
    return new sad.Point3I(fn(this.x, o.x), fn(this.y, o.y), fn(this.z, o.z));
};

sad.internal.makeMathFns(sad.Point3I.prototype);

sad.Point3I.prototype.toString = function() {
    return "sad::Point3I(" + this.x + ',' + this.y + ',' + this.z + ')';
};

// sad.p2d.Vector bindings

sad.p2d = {};
sad.p2d.Vector = SadP2DVector;


sad.p2d.Vector.prototype.to3d = function() {
    return new sad.Point3D(this.x, this.y, 0);
};

sad.p2d.Vector.prototype.to3i = function() {
    return new sad.Point3I(this.x, this.y, 0);
};

sad.p2d.Vector.prototype.to2i = function() {
    return new sad.Point2I(this.x, this.y);
};

sad.p2d.Vector.prototype.makeWithFn = function(fn, o) {
    return new sad.Point2D(fn(this.x, o.x), fn(this.y, o.y));
};

sad.internal.makeMathFns(sad.p2d.Vector.prototype);

sad.p2d.Vector.prototype.toString = function() {
    return "sad::p2d::Vector(" + this.x + ',' + this.y + ')';
};

sad.p2d.modulo = SadP2DModulo;
sad.p2d.basis = SadP2DBasis;
sad.p2d.unit = SadP2DUnit;
sad.p2d.scalar = SadP2DScalar;
sad.p2d.ortho = SadP2DOrtho;
sad.p2d.OVI_DEG_90 = 0;
sad.p2d.OVI_DEG_270 = 1;

// sad.Size2D

sad.Size2D = SadSize2D;
sad.Size2D.prototype.to2i = function() {
    return new sad.Size2I(this.width, this.height);
};
sad.Size2D.prototype.toString = function() {
    return "sad::Size2D(" + this.width + ',' + this.height + ')';
};

// sad.Size2I

sad.Size2I = SadSize2I;
sad.Size2I.prototype.to2d = function() {
    return new sad.Size2D(this.width, this.height);
};
sad.Size2I.prototype.toString = function() {
    return "sad::Size2I(" + this.width + ',' + this.height + ')';
};

// sad.Rect2D

sad.Rect2D = SadRect2D;

sad.Rect2D.prototype.to2i = function() {
    return new sad.Rect2I(this.p0().to2i(), this.p1().to2i(), this.p2().to2i(), this.p3().to2i());
};
sad.Rect2D.prototype.toString = function() {
    var result = [];
    for(var i = 0; i < 4; i++)
    {
        result.push(sad.Point2D.prototype.toString.call(this['p' + i]()));
    }
    return "sad::Rect2D(" +  result.join(', ') + ')';
};

// sad.Rect2I

sad.Rect2I = SadRect2I;

sad.Rect2I.prototype.to2d = function() {
    return new sad.Rect2D(this.p0().to2d(), this.p1().to2d(), this.p2().to2d(), this.p3().to2d());
};
sad.Rect2I.prototype.toString = function() {
    var result = [];
    for(var i = 0; i < 4; i++)
    {
        result.push(sad.Point2I.prototype.toString.call(this['p' + i]()));
    }
    return "sad::Rect2I(" +  result.join(', ') + ')';
};


// console object

var console = {};
console.dump = function(o) {
    var type = typeof o;
    if (type == "undefined") { return "undefined"};
    if ((type == "boolean") || (type == "number") || (type == "string") || (type == "symbol") || (type == "function")) { return o.toString();}
    if (type == "object")
    {
        if (o === null)
        {
            return "null";
        }
        if (o instanceof Array)
        {
            var  i = 0; 
            var  result = [];
            for(var i = 0; i < o.length; i++)
            {
                if (i in o)
                {
                    result.push(console.dump(o[i]));
                }
            }
            return "[" + result.join(", ") + "]";
        }
        else
        {
            if (SadInternalIsNativeObject(o))
            {
                if (o.hasOwnProperty('toString'))
                {
                    return o.toString();
                } 
                var hasPrototypeToString = false;
                if (typeof o.prototype != "undefined")
                {
                    hasPrototypeToString = (o.prototype.hasOwnProperty('toString') && o.prototype.toString != Object.toString);
                }
                if (hasPrototypeToString)
                {
                    return o.toString();
                }
                return SadInternalDumpNativeObject(o);
            }
            else
            {
                if (o.hasOwnProperty('toString'))
                {
                    return o.toString();
                }
                else
                {
                    var asString = o.toString();
                    if (asString != "[object Object]")
                    {
                        return asString;
                    }
                    else
                    {
                        var result = [];
                        for(var key in o) { result.push("\"" + key + "\"" + ' : ' + console.dump(o[key]));}
                    }
                }
                return "{" + result.join(", ") + "}";
            }
        }
    }
    return "";
};

console.log = function(o)
{
    print(console.dump(o));
};

// sad.util and sad.fs
sad.fs = {};

sad.fs.pathDelimiter = SadUtilPathDelimiter;
sad.fs.canonicalizePath = SadUtilCanonicalizePath;
sad.fs.isAbsolutePath = SadUtilIsAbsolutePath;
sad.fs.concatPaths = SadUtilConcatPaths;
sad.fs.folder = SadUtilFolder;
sad.fs.fileExists = SadUtilFileExists;
sad.internal.copyObject(sad.util, sad.fs);

// sad.spit and sad.slurp

sad.spit = SadSpit;
sad.slurp = SadSlurp;

sad.spitJSON = function(a, obj) {
    return sad.spit(a, JSON.stringify(obj, null, 2));
};

sad.slurpJSON = function(o) {
    var  r = sad.slurp(o);
    if (r !== null) {
        return JSON.parse(r);
    }
    return null;
};


// context

context.evalFromFile = function(string) {
    var s = sad.slurp(string);
    if (s == null) {
        throw new Error("Cannot read file \"" + string + "\"");
    }
    return eval(s);
};

context.eval = function(string) {
    return eval(string);
};

sad.Context = SadContext;

sad.Context.prototype.eval = function(string) {
    return SadContextEval(this, string);
};

sad.Context.prototype.evalFromFile = function(string) {
    return SadContextEvalFromFile(this, string);
};

// sad.Renderer bindings

sad.Renderer = SadRenderer;

sad.Renderer.ref = SadRendererRef;
sad.Renderer.makeFPSInterpolationDefault = function() {  SadInternalMakeFPSInterpolationDefault(this); }
sad.Renderer.makeFPSInterpolationObjectDependent = function() {  SadInternalMakeFPSInterpolationObjectDependent(this); }

// sad.Settings bindings

sad.Settings = SadSettings;

// sad.Timer bindings

sad.Timer = SadTimer;

sad.Timer.prototype.elapsedInMs = function() {
    this.stop();
    return this.elapsed();
};

// sad.Thread bindings

sad.Thread = SadThread;

// sad.Color bindings

sad.Color = SadColor;

sad.internal.uCharToInt = function(a) {
    if (a.length == 0) {
        return 0;
    }
    return a[0].charCodeAt(0);
}

sad.Color.prototype.toString = function() {
    var o =  [ sad.internal.uCharToInt(this.r),  sad.internal.uCharToInt(this.g), sad.internal.uCharToInt(this.b) ]; 
    return "sad::Color(" + o.join(', ')  + ")";
}

sad.AColor = SadAColor;

sad.AColor.prototype.toString = function() {
    var o =  [ sad.internal.uCharToInt(this.r),  sad.internal.uCharToInt(this.g), sad.internal.uCharToInt(this.b), sad.internal.uCharToInt(this.a) ]; 
    return "sad::AColor(" + o.join(', ')  + ")";
}

// sad.log.Log bindings

sad.log = {};
sad.log.Log = SadLog;
sad.log.Log.prototype.ref = SadLogRef;
sad.log.Log.prototype.addTarget = function(a) { SadInternalLogAddTarget(this, a); }
sad.log.Log.prototype.removeTarget = function(a) { SadInternalLogRemoveTarget(this, a); }

sad.log.Priority = {
    "FATAL" : 6, 
    "CRITICAL" : 5, 
    "WARNING" : 4,
    "MESSAGE" : 3, 
    "DEBUG" : 2, 
    "USER" : 1, 
    "SADDY_INTERNAL" : 0  
};

sad.log.ConsoleTarget = SadLogConsoleTarget;
sad.log.FileTarget = SadLogFileTarget;

sad.sleep = SadSleep;
sad.Mutex = SadMutex;
sad.Semaphore = SadSemaphore;

sad.MouseCursor = SadMouseCursor;


sad.resource = {};
sad.resource.Tree = SadResourceTree;

sad.resource.Tree.prototype.ref = function() {
    return sad.Renderer.ref().tree("");
}