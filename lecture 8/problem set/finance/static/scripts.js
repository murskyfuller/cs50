(function() {
    $(document).ready(function() {
        // for each form, add an onsubmit validity check
        $('form').each(function() {
            const form = $(this);
            form.submit(function(event) {
                let result = true;

                if (!this.checkValidity()) {
                    form.find("input,select,fieldset,output,textarea").each(function() {
                        const input = $(this);
                        const name = input.attr('name');
                        const feedback = $(`#${name}-feedback`);

                        if (!feedback) {
                            // let the dev know they don't have a feedback element to display the message
                            console.warn(`No feedback element provided for input: ${name}`);
                            return;
                        }

                        // check that the confirmation element in register matches the password element correctly
                        if (this.name === 'confirmation') {
                            const password = document.querySelector("input[name='password']");
                            if (this.value && this.value !== password.value) {
                                this.setCustomValidity("Does not match password");
                            } else if (this.value) {
                                // clear any potential custom validity message to mark input as valid
                                this.setCustomValidity("");
                            }
                        }

                        let message = 'Looks good!';
                        if (this.validationMessage) {
                            if (this.type === 'password' && this.validity.patternMismatch) {
                                message =
                                    "Password must be at least 6 characters and include:<br />one number<br />one upper case letter<br />one special character";
                            } else {
                                message = this.validationMessage;
                            }

                            // input is invalid
                            input.removeClass('valid');
                            input.addClass('invalid');
                            feedback.removeClass('valid');
                            feedback.addClass('invalid');
                            result = false;
                        } else {
                            //input is valid
                            input.removeClass('invalid');
                            input.addClass('valid');
                            feedback.removeClass('invalid');
                            feedback.addClass('valid');
                        }

                        // set the feedback element's html to
                        feedback.html(message);
                    });

                    form.addClass('validated'); // form is now validated either way
                }

                return result;
            });
        });
    });
})();