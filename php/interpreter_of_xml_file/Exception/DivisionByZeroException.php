<?php
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class DivisionByZeroException extends IPPException {
    public function __construct(?\Throwable $previous = null, bool $showTrace = true) {
        parent::__construct("Division by zero", ReturnCode::OPERAND_VALUE_ERROR, $previous, $showTrace);
    }
}