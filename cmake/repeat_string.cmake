# * returns a single string made of a string repeated n times.
#
# repeat_string(<string> <times> <separator> <outvar>)
#
# Returns a string in <outvar> built concatenating the given <string> a number of
# times equals to <times>. The single <string> instances are separated by a
# single <separator>.
#
# cmake-format: off
# Copyright 2018 Federico Ficarelli
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# cmake-format: on

if(__repeat_string_defined)
  return()
endif()
set(__repeat_string_defined ON)

function(repeat_string _string _times _separator _outvar)
  set(result "")
  math(EXPR count "${_times}")
  if(count GREATER "0")
    # First time doesn't need separator
    set(result "${_string}")
    math(EXPR count "${count} - 1")
    while(count GREATER "0")
      string(CONCAT result "${result}" "${_separator}${_string}")
      math(EXPR count "${count} - 1")
    endwhile()
  endif()
  set(${_outvar} ${result} PARENT_SCOPE)
endfunction(repeat_string)
