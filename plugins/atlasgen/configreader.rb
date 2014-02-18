##
# :title: configreader.rb
# A source file for reading an input config from file as
# XML document


require 'devil'
require 'rexml/document'
require 'rexml/element'

load 'spriteconfig.rb'

##
# :category: Public classes
# A class, that reads a config and source images and returns some _SpriteConfig_ objects or _nil_,
# if cannot read source file
class XMLConfigReader
    private
    ##
    # _TrueClass_ or _FalseClass_. True if one successfull reading was performed
    attr_writer :computed
    ##
    # _SpriteConfig_. A read config data (nil if no reading where performed or failed)
    attr_writer :config
    ## 
    # _Array_ of _String_.   Array of strings
    attr_writer :errors
    ##
    # _String_.  Name of file for output config 
    attr_writer :outputConfig
    ##
    # _String_.  Name of file, where ouput texture should be stored
    attr_writer :outputTexture
    
   public
    
    ##
    # _TrueClass_ or _FalseClass_. True if one successfull reading was performed
    attr_reader :computed
    
    ##
    # :category: Public interface
    # The default reader is broken, every attempt to get some config data like +getOutputConfigName()+ or +getOutputTextureName()+ leads to
    # exception
    # A +errors+ field is empty array. +computed+ is false. 
    def initialize()
        @computed = false
        @config = nil
        @errors = Array.new()
        @outputConfig = nil
        @outputTexture = nil
    end
    
    ##
    # :category: Implementation. DEPRECATED for use.
    # Reads a document from parent REXML::Element, filling a errors field on every error
    # Checks for duplicates and source image existence
    # [root]   _REXML::Element_ root element, where reading should be performed from
    # [return] _SpriteConfig_   a resulting sprite config. nil on error.
    def readDocument(root)
        # Get output texture name from attribute 'texture'
        if (root.attributes['texture'] == nil)
            @errors = @errors <<  "Output texture name is not defined"
        else
            @outputTexture = root.attributes['texture']
        end
        # Get output config name from attribute 'config'
        if (root.attributes['config'] == nil)
            @errors = @errors <<  "Output config is not defined"
        else
            @outputConfig = root.attributes['config']
        end
        # Try to read a config, scanning each subelement
        result = SpriteConfig.new()
        root.elements.each{
                |element|
                # Reads a new subelement and add to config
                # Element adds to texturearray source image, from +ConfigEntry::read+ by calling +TextureArray::pushUnique+
                tmp = ConfigEntry.new()
                errors = self.readElement(tmp, element,result)
                # Merges array of errors
                @errors = @errors + errors
                # Checks for unique items
                if (result.hasEntry(tmp.name, tmp.index))
                    @errors = @errors << ("Entry with name " + tmp.getFullName() + " already exists")
                else
                    result.pushEntry(tmp)
                end
        }
        # Check, whether errors were found
        if (@errors.length == 0)
            @computed = true
            return result
        else
            return nil
        end
    end
	
	##
    # :category: Public interface
    # Reads an XML element to entry. Does not add self to a config, instead loads a source image if possible
    # [tmp]     _ConfigEntry_     a sprite config entry
	# [element] _REXML::Element_  an element, which is mapped to entry
    # [config]  _SpriteConfig_    a resulting config
    # [return]  _Array_ of _String_ a errors, empty array if nothing found
    def readElement(tmp, element,config)
       errors = []
       # Parse name and index
       tmp.name = element.name
       if (element.attributes['index'] != nil)
            tmp.index = element.attributes['index'].to_i
       end
       # Parse texture
       if (element.attributes['texture'] != nil)
            tmp.inputTextureName = element.attributes['texture'] 
            if (config.getTextures().containsTexture(tmp.inputTextureName) == false)
                texture = Texture.new(tmp.inputTextureName)
                # If texture is loaded successfully
                if (texture.load())
                    config.getTextures().pushUnique(texture)
                else
                    errors = errors << ("Can't load texture with name " + tmp.inputTextureName)
                end
            end
       else
            errors = errors << ("At element with name " + tmp.getFullName() + " texture is not specified" )
       end
       # Parse size
       if (element.attributes['size'] != nil)
            list = element.attributes['size'].split(';')
            if (list.length!=2)
                errors = errors << ("At element with name " + tmp.getFullName() + " size must be defined as \"width;height\", but defined incorrectly")
            else
                tmp.size = [ list[0].to_i(), list[1].to_i() ]
            end
       end
       # Parse transparency
       if (element.attributes['transparent'] != nil)
            list = element.attributes['transparent'].split(';')
            if (list.length!=3)
                errors = errors << ("At element with name " + tmp.getFullName() + " transparency mask color must be defined as \"r;g;b\", but defined incorrectly")
            else
                tmp.transparent = [ list[0].to_i(), list[1].to_i(), list[2].to_i() ]
            end
       end
       return errors
    end
    ##
    # :category: Public interface
    # Reads a document from file, filling array of errors
    # Checks for duplicates and source image existence
    # [filename] _String_         name of file, where reading will be performed
    # [return]   _SpriteConfig_   a resulting sprite config. nil on error.
    def read(filename) 
        @computed = false
        @errors = []
        
        defaultMesg = [ "File error: Cannot open file: " + filename ]
        parseMesg = [ "Parsing error: Cannot read file: " + filename ]
        begin
            file = File.new(filename, "r")
            doc = REXML::Document.new file
            if (doc.root!=nil)
                return self.readDocument(doc.root)
            else
                 @errors = parseMesg
            end
        rescue Errno::ENOENT
            @errors = defaultMesg
        rescue Errno::EBADF
            @errors = defaultMesg
        rescue Errno::EINVAL
            @errors = defaultMesg
        rescue REXML::ParseException
            @errors = parseMesg
        end
        
        return nil
    end
    
    ##
    # :category: Public interface
    # Returns, whether last call of +read()+ was successfull
    # [return]  _TrueClass_ or _FalseClass_  whether reading was successfull
    def hasSucessfullReading?() 
        return @computed
    end
    
    ##
    # :category: Public interface
    # Returns last read config. Rises exception if reading was not successfull
    # [return]  _SpriteConfig_  last read config. 
    def getConfig()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @config
    end
    
    ##
    # :category: Public interface
    # Returns an output config filename
    # [return]  _String_  filename for output config.  
    def getOutputConfigName()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @outputConfig
    end
    
    ##
    # :category: Public interface
    # Returns an output texture filename
    # [return]  _String_  filename for output texture.  
    def getOutputTextureName()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @outputTexture
    end
    
    ##
    # :category: Public interface
    # Returns an array for errors
    # [return]  _Array_ of _String_  A error list.  
    def getErrors() 
        return @errors
    end
end

##
# :category: Public classes
# A class, that reads a config and source images and returns some _SpriteConfig_ objects or _nil_,
# if cannot read source file
class JSONConfigReader
    private
    ##
    # _TrueClass_ or _FalseClass_. True if one successfull reading was performed
    attr_writer :computed
    ##
    # _SpriteConfig_. A read config data (nil if no reading where performed or failed)
    attr_writer :config
    ## 
    # _Array_ of _String_.   Array of strings
    attr_writer :errors
    ##
    # _String_.  Name of file for output config 
    attr_writer :outputConfig
    ##
    # _String_.  Name of file, where ouput texture should be stored
    attr_writer :outputTexture
    
   public
    
    ##
    # _TrueClass_ or _FalseClass_. True if one successfull reading was performed
    attr_reader :computed
    
    ##
    # :category: Public interface
    # The default reader is broken, every attempt to get some config data like +getOutputConfigName()+ or +getOutputTextureName()+ leads to
    # exception
    # A +errors+ field is empty array. +computed+ is false. 
    def initialize()
        @computed = false
        @config = nil
        @errors = Array.new()
        @outputConfig = nil
        @outputTexture = nil
    end
    
    ##
    # :category: Implementation. DEPRECATED for use.
    # Reads a document from parent REXML::Element, filling a errors field on every error
    # Checks for duplicates and source image existence
    # [root]   _Hash_ root element, where reading should be performed from
    # [return] _SpriteConfig_   a resulting sprite config. nil on error.
    def readDocument(root)
        # Get output texture name from attribute 'texture'
        if (root.key?('texture') == false)
            @errors = @errors <<  "Output texture name is not defined"
        else
            @outputTexture = root['texture']
        end
        # Get output config name from attribute 'config'
        if (root.key?('config') == false)
            @errors = @errors <<  "Output config is not defined"
        else
            @outputConfig = root['config']
        end
		
		
		
		# Try to read a config, scanning each subelement
        result = SpriteConfig.new()
		
		# Get resource texture name
		if (root.key?('resource') == false)
            @errors = @errors <<  "Output resource is not defined"
        else
            result.textureResourceName = root['resource']
        end
        
        if (root.key?('sprites') == false)
            @errors = @errors <<  "Sprites is not defined"
        else
			if (root['sprites'].is_a?(Array))
				root['sprites'].each{
					|element|
					# Reads a new subelement and add to config
					# Element adds to texturearray source image, from +ConfigEntry::read+ by calling +TextureArray::pushUnique+
					tmp = ConfigEntry.new()
					errors = self.readElement(tmp, element,result)
					# Merges array of errors
					@errors = @errors + errors
					# Checks for unique items
					if (result.hasEntry(tmp.name, tmp.index))
						@errors = @errors << ("Entry with name " + tmp.getFullName() + " already exists")
					else
						result.pushEntry(tmp)
					end
				}
			else
				@errors = @errors <<  "Invalid sprites value"
			end
        end
		
        
        # Check, whether errors were found
        if (@errors.length == 0)
            @computed = true
            return result
        else
            return nil
        end
    end
	
	##
    # :category: Public interface
    # Reads an XML element to entry. Does not add self to a config, instead loads a source image if possible
    # [tmp]     _ConfigEntry_     a sprite config entry
	# [element] _Hash_  an element, which is mapped to entry
    # [config]  _SpriteConfig_    a resulting config
    # [return]  _Array_ of _String_ a errors, empty array if nothing found
    def readElement(tmp, element, config)
       errors = []
	   if (element.is_a?(Hash) == false)
			return [ "Found non-hash element in sprite list"]
	   end
       # Parse name and index
	   if (element.key?('name'))
            tmp.name = element['name']
       end
       if (element.key?('index'))
            tmp.index = element['index'].to_i
       end
       # Parse texture
       if (element.key?('texture'))
            tmp.inputTextureName = element['texture'] 
            if (config.getTextures().containsTexture(tmp.inputTextureName) == false)
                texture = Texture.new(tmp.inputTextureName)
                # If texture is loaded successfully
                if (texture.load())
                    config.getTextures().pushUnique(texture)
                else
                    errors = errors << ("Can't load texture with name " + @inputTextureName)
                end
            end
       else
            errors = errors << ("At element with name " + tmp.getFullName() + " texture is not specified" )
       end
       # Parse size
       if (element.key?('size'))
            list = element['size'].split(';')
            if (list.length!=2)
                errors = errors << ("At element with name " + tmp.getFullName() + " size must be defined as \"width;height\", but defined incorrectly")
            else
                tmp.size = [ list[0].to_i(), list[1].to_i() ]
            end
       end
       # Parse transparency
       if (element.key?('transparent'))
            list = element['transparent'].split(';')
            if (list.length!=3)
                errors = errors << ("At element with name " + tmp.getFullName() + " transparency mask color must be defined as \"r;g;b\", but defined incorrectly")
            else
                tmp.transparent = [ list[0].to_i(), list[1].to_i(), list[2].to_i() ]
            end
       end
       return errors
    end
    ##
    # :category: Public interface
    # Reads a document from file, filling array of errors
    # Checks for duplicates and source image existence
    # [filename] _String_         name of file, where reading will be performed
    # [return]   _SpriteConfig_   a resulting sprite config. nil on error.
    def read(filename) 
        @computed = false
        @errors = []
        
        defaultMesg = [ "File error: Cannot open file: " + filename ]
        parseMesg = [ "Parsing error: Cannot read file: " + filename ]
        begin
            file = File.new(filename, "r")
            doc = JSON.load file
            if (doc.is_a?(Hash))
                return self.readDocument(doc)
            else
                 @errors = parseMesg
            end
        rescue Errno::ENOENT
            @errors = defaultMesg
        rescue Errno::EBADF
            @errors = defaultMesg
        rescue Errno::EINVAL
            @errors = defaultMesg
		rescue JSON::ParserError
			@errors = parseMesg
        end
        
        return nil
    end
    
    ##
    # :category: Public interface
    # Returns, whether last call of +read()+ was successfull
    # [return]  _TrueClass_ or _FalseClass_  whether reading was successfull
    def hasSucessfullReading?() 
        return @computed
    end
    
    ##
    # :category: Public interface
    # Returns last read config. Rises exception if reading was not successfull
    # [return]  _SpriteConfig_  last read config. 
    def getConfig()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @config
    end
    
    ##
    # :category: Public interface
    # Returns an output config filename
    # [return]  _String_  filename for output config.  
    def getOutputConfigName()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @outputConfig
    end
    
    ##
    # :category: Public interface
    # Returns an output texture filename
    # [return]  _String_  filename for output texture.  
    def getOutputTextureName()
        if (@computed == false)
            raise 'Error: reading was not successfull'
        end
        return @outputTexture
    end
    
    ##
    # :category: Public interface
    # Returns an array for errors
    # [return]  _Array_ of _String_  A error list.  
    def getErrors() 
        return @errors
    end
end