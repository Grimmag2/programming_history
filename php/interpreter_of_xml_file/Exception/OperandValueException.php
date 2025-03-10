<?php
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class OperandValueException extends IPPException {
    public function __construct(?\Throwable $previous = null, bool $showTrace = true) {
        parent::__construct("Value of operand is out of range", ReturnCode::OPERAND_VALUE_ERROR, $previous, $showTrace);
    }
}