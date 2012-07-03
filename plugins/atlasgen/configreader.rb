# A reader, that used to load some config is placed here.# It loads config from XML and also some textures into cairo contextsrequire 'cairo'require 'rexml/document'require 'rexml/element'load 'spriteconfig.rb'# A reader, that reads some config from file and reads it to fully functional config# Also reads some textures. Each time, config is being read, it kept in there, and old config must# be savedclass ConfigReader    private        # Boolean. This field stores, whether some one successfull reading was performed    attr_writer :computed    # SpriteConfig.  Inner loaded config    attr_writer :config    # Array.   Array of strings    attr_writer :errors    # String.  Name of output config     attr_writer :outputConfig    # String.  Name of ouput texture    attr_writer :outputTexture       public        # Boolean. This field stores, whether some one successfull reading was performed    attr_reader :computed        # The default reader is created, as broken and every try to read from it leads to exception    def initialize()        @computed = false        @config = SpriteConfig.nil        @errors = Array.new()        @outputConfig = String.nil        @outputTexture = String.nil    end        # Reads a config from file. Returns config or null on failure    #  * param String filename    #  * return Config object     def read(filename)         @computed = false        return SpriteConfig.nil    end        # Returns, whether last time reading of config was successfull    # * return Boolean whether reading was successfull or not.    def hasSucessfullReading?()         return @computed    end        # Returns read config    # * return Config read config    def getConfig()        if (@computed == false)            raise 'Error: reading was not successfull'        end        return @config    end        # Returns an output config name    # * return Sting output config name    def getOutputConfigName()        if (@computed == false)            raise 'Error: reading was not successfull'        end        return @outputConfig    end        # Returns an output texture name    # * return Sting output texture name    def getOutputTextureName()        if (@computed == false)            raise 'Error: reading was not successfull'        end        return @outputTexture    end        # Returns an array of errors    # * return Array error array    def getErrors()         return @errors    endend