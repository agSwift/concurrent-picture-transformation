#!/usr/bin/env ruby

require 'json'
require 'benchmark'

# test result array (for JSON output)
@testscores = []

# test time hash (for speed analysis)
@test_times = {}

#####################################################################

# SUPPORT FUNCTIONS:

def run_test(test_name, pre_load, actual_images, expected_images, expected_outputs=[], not_expected_outputs=[])
  # report misconfugure test case
  if actual_images.length != expected_images.length then
    puts "Error: invalid supplied test data"
    return
  end
  # run the picture library on the supplied test file input and capture execution time
  puts "> running: #{test_name}"
  puts "--------------------------------------"
  puts "run concurrent picture library:"
  actual = ""
  time = Benchmark.realtime do
    actual = %x(./concurrent_picture_lib #{pre_load} < test_files/#{test_name}.txt 2>&1)
  end
  test_success = $?.exitstatus == 0 
  puts actual
  puts "clean exit: #{test_success}"
  test_metadata = {"time": time, "pass": test_success}
  
  # catch test case errors
  if(!test_success) then
    puts "  - concurrent picture library reported non-zero exit code!"
    @testscores << {"score": 0, "name": "#{test_name}", "possible": 1}
    @test_times[test_name] = test_metadata
    puts ""
    return    
  end
  
  # check if expected output included somewhere in actual output
  expected_outputs.each do |substr|
    if(!actual.include?(substr)) then
      puts "  - concurrent picture library did not include #{substr} in terminal output."
      @testscores << {"score": 0, "name": "#{test_name}", "possible": 1}
      test_metadata[:pass] = false
      @test_times[test_name] = test_metadata
      puts ""
      return
    end
  end
  
  #check that not_expected_outputs are not included anywhere in actual output
  not_expected_outputs.each do |substr|
    if(actual.include?(substr)) then
      puts "  - concurrent picture library should not include #{substr} in terminal output"
      @testscores << {"score": 0, "name": "#{test_name}", "possible": 1}
      test_metadata[:pass] = false
      @test_times[test_name] = test_metadata
      puts ""
      return      
    end
  end
  
  puts ""
  
  # check final images same as expected images
  puts "check final state of images:"
  actual_images.each_with_index do |image, index|
    system %Q(./picture_compare test_images/#{image} test_images/#{expected_images[index]} 2>&1)
    test_success = $?.exitstatus == 0 
    test_metadata[:pass] = test_success
    if(!test_success) then
      puts "  - picture comparison failed for #{image}"
      @testscores << {"score": 0, "name": "#{test_name}", "possible": 1}
      @test_times[test_name] = test_metadata
      puts ""
      return
    end    
  end
  
  # report success only if all checks passed
  puts "  + all final images correct"
  @testscores << {"score": 1, "name": "#{test_name}", "possible": 1}
  @test_times[test_name] = test_metadata
  puts ""
end


#####################################################################

# MAIN PROGRAM START:

system %Q(make 2>&1)
if($?.exitstatus != 0) then
  puts "Failed to compile!"
  @testscores << {"score": 0, "name": "Compilation", "possible": 1}
else

  puts "------------------------------"
  puts "   Basic Interpreter Tests    " 
  puts "------------------------------"
  puts ""  

  # basic lib-command tests (check the output for specific contents):   
  run_test("exit_test","test_images/ducks1.jpg test_images/ducks2.jpg",[],[],[],["ducks1\n"]) #exit
  run_test("empty_input","",[],[]) #empty line robustness
  run_test("liststore","test_images/ducks1.jpg test_images/ducks2.jpg test_images/ducks3.jpg",[],[],["ducks1\n", "ducks2\n", "ducks3\n"]) #liststore
  run_test("load_test","",[],[],["funny_name"]) #load
  run_test("unload_test","test_images/ducks2.jpg test_images/ducks1.jpg test_images/test.jpg",[],[],["ducks1\n"],["ducks2\n"]) #unload
  run_test("save_test","test_images/some_ducks.jpg",["a_random_test_name.jpg"],["a_random_test_name.jpeg"]) #save  
    
  # basic "sequential" transformation tests:
  run_test("test_invert", "test_images/test.jpg", ["test_inverted.jpg"], ["test_inverted.jpeg"])
  run_test("test_load_and_invert", "", ["test_inverted.jpg"], ["test_inverted.jpeg"])

  run_test("test_grayscale", "test_images/test.jpg", ["test_grayscale.jpg"], ["test_grayscale.jpeg"])
  run_test("test_load_and_grayscale", "", ["test_grayscale.jpg"], ["test_grayscale.jpeg"])

  run_test("test_rotate_90", "test_images/test.jpg", ["test_rotate_90.jpg"], ["test_rotate_90.jpeg"])
  run_test("test_load_and_rotate_90", "", ["test_rotate_90.jpg"], ["test_rotate_90.jpeg"])

  run_test("test_rotate_180", "test_images/test.jpg", ["test_rotate_180.jpg"], ["test_rotate_180.jpeg"])
  run_test("test_load_and_rotate_180", "", ["test_rotate_180.jpg"], ["test_rotate_180.jpeg"])

  run_test("test_rotate_270", "test_images/test.jpg", ["test_rotate_270.jpg"], ["test_rotate_270.jpeg"])
  run_test("test_load_and_rotate_270", "", ["test_rotate_270.jpg"], ["test_rotate_270.jpeg"])

  run_test("test_flipH", "test_images/test.jpg", ["test_flip_H.jpg"], ["test_flip_H.jpeg"])
  run_test("test_load_and_flipH", "", ["test_flip_H.jpg"], ["test_flip_H.jpeg"])

  run_test("test_flipV", "test_images/test.jpg", ["test_flip_V.jpg"], ["test_flip_V.jpeg"])  
  run_test("test_load_and_flipV", "", ["test_flip_V.jpg"], ["test_flip_V.jpeg"])

  run_test("test_blur", "test_images/test.jpg", ["test_blur.jpg"], ["test_blur.jpeg"])
  run_test("test_load_and_blur", "", ["test_blur.jpg"], ["test_blur.jpeg"])  
      
  # basic concurrency tests (check thread-safe and actual speed-up):
  puts "------------------------------"
  puts "   Basic Concurrency Tests    "
  puts "------------------------------"
  puts ""  
  run_test("concurrent_blurs", "", ["test_blur1.jpg", "test_blur2.jpg", "test_blur3.jpg", "test_blur4.jpg", "test_blur5.jpg",
                                    "test_blur6.jpg", "test_blur7.jpg", "test_blur8.jpg", "test_blur9.jpg", "test_blur10.jpg"], 
                                   ["test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg",
                                    "test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg", "test_blur.jpeg"])  
  puts ""
  puts "execution time for 01 blur transformations = #{@test_times["test_load_and_blur"][:time]*1000}"
  puts "   target time for 10 blur transformations = #{@test_times["test_load_and_blur"][:time]*5000}"
  puts "execution time for 10 blur transformations = #{@test_times["concurrent_blurs"][:time]*1000}"
  puts ""   
  
  if (@test_times["test_load_and_blur"][:pass] & @test_times["concurrent_blurs"][:pass]) then
    if (@test_times["concurrent_blurs"][:time] < @test_times["test_load_and_blur"][:time]*5) then
      puts "concurrent execution has achieved at least 2x speedup"
      @testscores << {"score": 1, "name": "speedup test", "possible": 1}
    else
      puts "concurrent execution has not achieved 2x speedup"
      @testscores << {"score": 0, "name": "speedup test", "possible": 1}
    end
  else
    puts "one or both of the blur tests failed, so unable to sample concurrent speedup"
    @testscores << {"score": 0, "name": "speedup test", "possible": 1}
  end
  puts ""


  # full integration tests (more realistic inputs):
  puts "------------------------------"
  puts "    Full Integration Tests    "
  puts "------------------------------"
  puts ""    
  run_test("test_10_blurs", "", ["test_10_blurs.jpg"], ["test_10_blurs.jpeg"])
  run_test("example_input", "", ["boring.jpg", "psychedelic_art.jpg", "spot_the_difference.jpg", "need_glasses.jpg", "ducks3.jpg"], 
                                ["boring.jpeg", "psychedelic_art.jpeg", "spot_the_difference.jpeg", "need_glasses.jpeg", "ducks3.jpeg"])    
  
end

# JSON Output #
#
# This file must have the following format:
#
#  [ {"score": 9, "name": "Basic Tests", "possible": 9}, 
#    {"score": 13, "name": "Advanced Tests", "possible": 13}, 
#    {"score": 1, "name": "Robustness Tests", "possible": 1},
#     ...etc...
#  ]
#

File.open('test_results_info.json', 'w') do |file|
  file.puts @testscores.to_json
end



