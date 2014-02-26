class LayerSet
  attr_accessor :name, :layer_names, :layer_sets, :parent

  def initialize(name, parent)
    @name = name
    @layer_names = []
    @layer_sets = []
    @parent = parent
  end

  def add(line)
    @layer_names << line
  end

  def add_layer_set(layer_set)
    @layer_sets << layer_set
  end

  def traverse(parent_name = nil)
    name = parent_name ? "#{parent_name}/#{@name}" : "#{@name}"
    @layer_sets.each do |layer_set|
      layer_set.traverse(name)
    end
    @layer_names.each do |layer_name|
      puts "#{name}/#{layer_name}"
    end
  end
end

class Traverser
  attr_accessor :layer_set, :parent, :root

  def initialize
    @layer_set = @parent = @root = LayerSet.new("", nil)
  end

  def run
    lines = open("tmp.txt", 'r:sjis:utf-8').readlines.reverse
    lines.each do |line|
      line.chomp!

      case
      when line == "<LayerSet>"
        @parent = @layer_set
        @layer_set = LayerSet.new(nil, parent)
        @parent.add_layer_set(@layer_set)
      when line == "</Layer set>" || line == "</Layer group>"
        @layer_set = layer_set.parent
        @parent = layer_set.parent
      when @layer_set.name.nil?
        @layer_set.name = line
      else
        @layer_set.add(line)
      end
    end

    @root.traverse
  end
end

Traverser.new.run
