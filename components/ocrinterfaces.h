#ifndef h_ocrinterfaces__
#define h_ocrinterfaces__

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
//
// You may not use this file except under the terms of the accompanying license.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Project: iulib -- image understanding library
// File: ocrinterfaces.h
// Purpose: interfaces to OCR system components
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file ocrinterfaces.h
/// \brief Interfaces to OCR system components


#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include "colib/narray.h"
#include "colib/narray-util.h"
#include "colib/smartptr.h"
#include "colib/misc.h"
#include "colib/coords.h"
#include "colib/iustring.h"
#include "iulib/components.h"

namespace ocropus {

    /// Base class for OCR interfaces.

    /// Cleanup for gray scale document images.

    /// Should work for both gray scale and binary images.
    ///
    struct ICleanupGray : virtual IComponent {
        const char *interface() { return "ICleanupGray"; }
        /// Clean up a gray image.
        virtual void cleanup_gray(bytearray &out,bytearray &in) { throw Unimplemented(); }
    };

    /// Cleanup for binary document images.

    /// Should throw an error when applied to grayscale.
    ///
    struct ICleanupBinary : virtual IComponent {
        const char *interface() { return "ICleanupBinary"; }
        /// Clean up a binary image.
        virtual void cleanup(bytearray &out,bytearray &in) { throw Unimplemented(); }
    };

    /// Perform binarization of grayscale images.

    struct IBinarize : virtual IComponent {
        const char *interface() { return "IBinarize"; }
        /// Binarize an image stored in a floatarray. Override this.
        /// virtual void binarize(bytearray &out,floatarray &in) = 0;
        /// \brief Binarize an image stored in a bytearray.
        /// Override this if you want to provide a more efficient
        /// implementation.
        virtual void binarize(bytearray &out,bytearray &in)  { throw Unimplemented(); }

        /// A second interface that simultaneoulsy yields a grayscale
        /// and a binary image.  This is important if the binarization process
        /// involves a step like deskewing (or if it enhances the grayscale
        /// image).
        virtual void binarize(bytearray &out,bytearray &gray,bytearray &in) {
            binarize(out,in);
            gray = in;
        }
    };

    /// Compute text/image probabilities

    /// The output is in the standard RGB format
    /// for text/image segmentation (see ocropus.org)

    struct ITextImageClassification : virtual IComponent {
        const char *interface() { return "ITextImageClassification"; }
        /// Compute text/image probabilities.
        virtual void textImageProbabilities(intarray &out,bytearray &in)  { throw Unimplemented(); }
    };

    /// Compute page segmentation into columns, lines, etc.

    /// The output is in the standard RGB format
    /// for page segmentation (see ocropus.org)

    struct ISegmentPage : virtual IComponent {
        const char *interface() { return "ISegmentPage"; }
        /// Segment the page.
        virtual void segment(intarray &out,bytearray &in)  { throw Unimplemented(); }
        virtual void segment(intarray &out,bytearray &in,rectarray &obstacles)  { throw Unimplemented(); }
    };

    /// Compute line segmentation into character hypotheses.
    //
    /// The output is in the standard RGB format
    /// for page segmentation (see ocropus.org)

    struct ISegmentLine : virtual IComponent {
        const char *interface() { return "ISegmentLine"; }
        /// Segment a line.
        virtual void charseg(intarray &out,bytearray &in)  { throw Unimplemented(); }
    };

    /// \brief A generic interface for language models.

    /// An IGenericFst is a directed graph
    /// with output/cost/id written on arcs,
    /// accept cost written on vertices and
    /// a fixed start vertice.
    struct IGenericFst : virtual IComponent {
        const char *interface() { return "IGenericFst"; }
        /// Clear the language model
        virtual void clear() { throw Unimplemented(); }

        /// Get a single new state
        virtual int newState() { throw Unimplemented(); }

        /// Add a transition between the given states
        virtual void addTransition(int from,int to,int output,float cost,int input) { throw Unimplemented(); }

        /// A variant of addTransition() with equal input and output.
        virtual void addTransition(int from,int to,int symbol,float cost) {
            addTransition(from, to, symbol, cost, symbol);
        }

        /// Set the start state
        virtual void setStart(int node) { throw Unimplemented(); }

        /// Set a state as an accept state
        virtual void setAccept(int node,float cost=0.0) { throw Unimplemented(); }

        /// Obtain codes for "specials" (language model dependent)
        virtual int special(const char *s) { throw Unimplemented(); }

        /// \brief Compute the best path through the language model.
        /// Useful for simple OCR tasks and for debugging.
        virtual void bestpath(ustrg &result) { throw Unimplemented(); }

        /// destroy the language model
        virtual ~IGenericFst() {}

        /// simple interface for line recognizers
        virtual void setString(ustrg &text,floatarray &costs,intarray &ids) {
            int n = text.length();
            intarray states;
            states.clear();
            for(int i=0;i<n+1;i++)
                states.push(newState());
            for(int i=0;i<n;i++)
                addTransition(states[i],states[i+1],text[i].ord(),costs[i],ids[i]);
            setStart(states[0]);
            setAccept(states[n]);
        }

        // reading methods

        /// Get the number of states.
        virtual int nStates() { throw Unimplemented(); }

        /// Get the starting state.
        virtual int getStart() { throw Unimplemented(); }

        /// Get the accept cost of a given vertex (a cost to finish the line and quit).
        virtual float getAcceptCost(int node) { throw Unimplemented(); }

        /// Determine whether the given node is an accepting state.
        virtual bool isAccepting(int node) { return getAcceptCost(node)<1e30; }

        /// Return an array of arcs leading from the given node.
        virtual void arcs(colib::intarray &ids,
                          colib::intarray &targets,
                          colib::intarray &outputs,
                          colib::floatarray &costs,
                          int from) { throw Unimplemented(); } // WARN_DEPRECATED

        /// A variant of addTransition() with equal input and output.
        virtual void getTransitions(intarray &tos,intarray &symbols,floatarray &costs,intarray &inputs,int from) {
            arcs(inputs,tos,symbols,costs,from);
        }

        /// Change a transition score between the given states
        virtual void rescore(int from,int to,int output,float new_cost,int input) { throw Unimplemented(); }

        /// A variant of rescore() with equal input and output.
        virtual void rescore(int from, int to, int symbol, float new_cost) {
            rescore(from, to, symbol, new_cost, symbol);
        }

        /// These methods should load and save in OpenFST format.
        /// (A simple way of doing that is to convert internally to OpenFST,
        /// then call its load/save methods.)
        virtual void load(const char *file) { throw Unimplemented(); }
        virtual void save(const char *file) { throw Unimplemented(); }
    };

#if 0
    /// A generic interface for isolated character recognizers.
    /// Note that this is not the preferred interface for character recognition,
    /// since feature extraction is quite inefficient if it's done a character at a time.

    struct ICharacterClassifier : virtual IComponent {
        const char *interface() { return "ICharacterClassifier"; }
        /// \brief Classify a character without any information about position on the line.
        ///
        /// May throw an exception if it's not implemented.
        virtual void setImage(bytearray &input_image) { throw Unimplemented(); }

        /// \brief Classify a character with information about position on the line.
        //
        /// May throw an exception if it's not implemented.
        virtual void setImage(bytearray &image,int base_y, int xheight_y, int descender_y, int ascender_y) { throw Unimplemented(); }

        /// Get the number of classes returned. Corresponds to indices to cls() and cost().
        virtual int length() { throw Unimplemented(); }

        /// Unicode character or character string.
        //
        /// Note that some classifiers may return multiple characters per class
        virtual void cls(ustrg &result, int i) { throw Unimplemented(); }

        /// cost value for this classification; lower costs = better
        /// should aim to return negative log likelihoods
        virtual float cost(int i) { throw Unimplemented(); }

        /// "adaptation" means temporary adaptation of the classifier
        /// to all the characters between startTraining and finishTraining
        /// other types of training are recognizer-dependent
        virtual void startTraining(const char *type="adaptation") { throw Unimplemented(); }

        /// Notify the classifier of the start of a new epoch.
        virtual void epoch(int n) {}

        /// \brief Train a character.
        //
        /// (Commonly, this only stores data in the model; training is via an external program).
        /// This may be also train on ligatures (if supported),
        /// that's why `characters' is a ustrg.
        virtual void addTrainingChar(bytearray &input_image,ustrg &characters)
            { throw Unimplemented(); }

        /// Train a character.
        virtual void addTrainingChar(bytearray &image,int base_y, int xheight_y, int descender_y,
                int ascender_y,ustrg &characters) { throw Unimplemented(); }

        /// Train a character in context (think about this some more).
        virtual void addTrainingChar(bytearray &image,bytearray &mask,ustrg &characters)
                { throw Unimplemented(); }

        /// Finish training and switch back to recognition; this method may
        /// take a long time to complete.
        virtual void finishTraining() { throw Unimplemented(); }

#if 0
        // inherited from IComponent

        // FIXME get rid of this

        /// Save a trained model to the stream.
        virtual void save(FILE *stream) { throw Unimplemented(); }
        void save(const char *path) { save(stdio(path, "wb")); }

        /// Load a trained model from the stream.
        virtual void load(FILE *stream) { throw Unimplemented(); }
        void load(const char *path) { load(stdio(path, "rb")); }
#endif

        /// \brief Convenience function for getting the best output
        //
        /// (useful for debugging)
        virtual void best(ustrg &result) {
            int mi = -1;
            float mc = 1e30;
            for(int i=0;i<length();i++) {
                if(cost(i)<mc) {
                    mi = i;
                    mc = cost(i);
                }
            }
            if(mi>=0)
                cls(result, mi);
            else
                result.clear();
        }

        /// destructor
        virtual ~ICharacterClassifier() {}
    };
#endif


    /// A generic interface for text line recognition.

    struct IRecognizeLine : virtual IComponent {
        const char *interface() {
            return "IRecognizeLine";
        }
        /// \brief Recognize a text line and return a lattice representing
        /// the recognition alternatives.
        virtual void recognizeLine(IGenericFst &result,bytearray &image) { throw Unimplemented(); }

        /// \brief Start training of the given type.

        /// "adaptation" means temporary adaptation of the classifier
        /// to all the lines between startTraining and finishTraining
        /// other types of training are recognizer-dependent
        virtual void startTraining(const char *type="adaptation") {
            throw Unimplemented();
        }

        /// \brief Train on a text line.

        /// Usage is: call addTrainingLine with training data, then call finishTraining
        ///
        /// The state of the object is undefined between calling
        /// addTrainingLine and finishTraining, and it is
        /// an error to call recognizeLine before finishTraining completes.
        /// This allows both batch and incemental training.
        ///
        /// The return value indicates whether additional training
        /// data is still desired by the classifier.
        ///
        /// NB: you might train on length 1 strings for single character training
        /// and might train on words if line alignment is not working
        /// (well, for some training data)

        virtual bool addTrainingLine(bytearray &image,ustrg &transcription) {
            throw Unimplemented();
        }


        /// \brief Train on a text line, given a segmentation.
        /// This is analogous to addTrainingLine(bytearray,ustrg) except that
        /// it takes the "ground truth" line segmentation.
        virtual bool addTrainingLine(intarray &segmentation, bytearray &image_grayscale,
                                     ustrg &transcription) {
            throw Unimplemented();
        }


        /// Align a lattice with a transcription.
        /// \param[out] chars Characters along the best path.
        //                    Currently, every character in chars must have a corresponding
        //                    region in seg and the characters must be in reading order.
        //                    Eventually, chars may contain characters (e.g., spaces) that
        //                    do not correspond to any region.  Note that chars may not
        //                    correspond to any string allowed/suggested by the transcription.
        /// \param[out] seg Aligned segmentation, colors correspond to chars (starting at 1)
        /// \param[out] costs Costs corresponding to chars
        /// \param[in] image Input grayscale image
        /// \param[in] transcription The "ground truth" lattice to align
        virtual void align(ustrg &chars,intarray &seg,floatarray &costs,
                           bytearray &image,IGenericFst &transcription) { throw Unimplemented(); }

        // eventually?
        // virtual void addTrainingLine(bytearray &image,IGenericFst &transcription) { throw Unimplemented(); }

        /// \brief Finish training, possibly making complex calculations.

        /// Call this when training is done and the system should switch back to recognition;
        /// this method may take a long time to complete.
        virtual void finishTraining() { throw Unimplemented(); }

        /// Notify the recognizer of the start of a new epoch (i.e.,
        /// if n>0, then we have seen the data before).
        virtual void epoch(int n) {}

        /// Destructor
        virtual ~IRecognizeLine() {}

        /// this is a weird, optional method that exposes character segmentation for those line recognizers that have it
        /// segmentation contains colored pixels, and a transition in
        /// the transducer of the form * --- 1/eps --> * --- 2/a --> *
        /// means that pixels with color 1 and 2 together form the
        /// letter "a"
        virtual void recognizeLine(intarray &segmentation,IGenericFst &result,bytearray &image)
            { throw Unimplemented(); }

        // recognize a line with or without a given segmentation
        // if useit is set to true, the given segmentation is just displayed in loggers, but not used,
        // the segmenter computes the segmentation and the recognition uses its output
        // if useit is set to false, the segmenter is still launched for the loggers, but the given
        // segmentation is really used for the recognition
        virtual void recognizeLineSeg(intarray &segmentation,IGenericFst &result,
                                      bytearray &image, bool useit)
            WARN_DEPRECATED { throw Unimplemented(); }
    };

    struct DoneTraining { };
}

#endif
