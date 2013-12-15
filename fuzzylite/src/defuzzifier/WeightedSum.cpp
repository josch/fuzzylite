/*   Copyright 2013 Juan Rada-Vilela

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "fl/defuzzifier/WeightedSum.h"

#include "fl/term/Accumulated.h"
#include "fl/term/Thresholded.h"
#include "fl/defuzzifier/Tsukamoto.h"

namespace fl {

    WeightedSum::WeightedSum() : Defuzzifier() {
    }

    WeightedSum::~WeightedSum() {
    }

    std::string WeightedSum::className() const {
        return "WeightedSum";
    }

    scalar WeightedSum::defuzzify(const Term* term,
            scalar minimum, scalar maximum) const {
        (void) minimum;
        (void) maximum;
        const Accumulated* takagiSugeno = dynamic_cast<const Accumulated*> (term);
        if (not takagiSugeno) {
            std::ostringstream ss;
            ss << "[defuzzification error]"
                    << "expected an Accumulated term instead of"
                    << "<" << term->toString() << ">";
            throw fl::Exception(ss.str(), FL_AT);
        }

        scalar sum = 0.0;
        for (int i = 0; i < takagiSugeno->numberOfTerms(); ++i) {
            const Thresholded* thresholded = dynamic_cast<const Thresholded*> (takagiSugeno->getTerm(i));
            if (not thresholded) {
                std::ostringstream ss;
                ss << "[defuzzification error]"
                        << "expected a Thresholded term instead of"
                        << "<" << takagiSugeno->getTerm(i)->toString() << ">";
                throw fl::Exception(ss.str(), FL_AT);
            }

            /**z is tsukamoto, and when it is takagi, it will not matter as it will be a function
            previously, takagi-sugeno was:
            sum += thresholded->getThreshold() * thresholded->getTerm()->membership(0);
            so replacing 0 with threshold w will give the same for takagi-sugeno, 
            plus provide tsukamoto**/

            scalar z = Tsukamoto::tsukamoto(thresholded, 
                    takagiSugeno->getMinimum(), takagiSugeno->getMaximum());

            sum += thresholded->getThreshold() * z;
        }

        return sum;
    }

    Defuzzifier* WeightedSum::constructor() {
        return new WeightedSum;
    }

}
