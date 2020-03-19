# Generic application 

This is the application main. It only runs the sysinit/app-core init and then the default event event q task.
No significant code should need to be added to this project - its all in the app-generic package

syscfg.yml/pkg.yml contains the base config/definition of the package; however it is the project's targets that contain the significant configuration elements, that customise the packages for each specific product and varient. 
Again, any config specific to any product should happen in the target subdirectory.

Finally, note that the BSP for this project is by default for a W_Proto card.

Any questions?  Please refer to the documentation at 
http://mynewt.apache.org/ or ask questions on dev@mynewt.apache.org
/**
 * Copyright 2019 Wyres
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, 
 * software distributed under the License is distributed on 
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
 * either express or implied. See the License for the specific 
 * language governing permissions and limitations under the License.
*/

