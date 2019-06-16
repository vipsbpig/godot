print("hello form lua world 中文 !!!")
--extends("extends call")

Object.new()
Node.new()

print("Finished!!!!")

obj = {}
obj.func = function(a, b, c)
    --print(a, b, c)
end
 
mtable = {__index = obj}
otherobj = {}
 
setmetatable(otherobj, mtable)
 
-- print("obj:", obj)
-- print("mtable", mtable)
-- print("otherobj:", otherobj)
-- otherobj.func(1, 2)
-- otherobj:func(1, 2)
