require 'werial'

class Gps
  def initialize
    @werial = Werial.new
  end

  def open(port)
    @werial.open(port)
  end

  def close
    @werial.close
  end

  def position
    data = @werial.data
    return nil if data.nil?

    data.each_line do |line|
      if line =~ /^\$GPGLL,(.+?)(..\..+?),(N|S),(.+?)(..\..+?),(E|W),(..)(..)(..),(A|V)\*.+$/
        @lat = ($1.to_i + $2.to_f/60)*($3 == 'N' ? 1 : -1)
        @lon = ($4.to_i + $5.to_f/60)*($6 == 'E' ? 1 : -1)

        @hour = $7.to_i
        @minute = $8.to_i
        @second = $9.to_i
      end
    end

    (@lat.nil? || @lon.nil?) ? nil : [@lat, @lon]
  end
end

if __FILE__ == $0
  g = Gps.new
  g.open('COM1')
  trap('INT') do
    g.close
    exit(0)
  end

  while true
    position = g.position
    puts position unless position.nil?
  end
end
